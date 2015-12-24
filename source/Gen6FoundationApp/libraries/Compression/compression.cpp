//======================================================================================
// Compression.c
//======================================================================================
// $RCSfile: compression.cpp $
// $Revision: 3776 $
// $Date: 2012-09-26 23:15:21 +0800 (Wed, 26 Sep 2012) $
//======================================================================================

//======================================================================================



//---------------------------------------------------------------------------
//
//  Module name:     COMPRESSION
//
//  Module description:
//
//---------------------------------------------------------------------------

#include "BitString.h"
#include "Compression.h"
#include "HHPCompressionConfig.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Definitions required by the JPEG library.  These MUST be included before JPEGLib.h.
//#define	XMD_H 					1
//#define	HAVE_UNSIGNED_CHAR		1
//#define	HAVE_UNSIGNED_SHORT		1
//#define	HAVE_BOOLEAN			1

// JPEG compression library interface.
#if (_WIN32_WCE || WIN32)
    #include "Windows.h"
#endif
extern "C"
{
#include "Deflate.h"
	    #include "JPEGLib.h"
#ifndef LZO_NOT_SUPPORTED
		#include	"lzo1x.h"
#endif

}






extern "C" int FatalJPEGErrorEncountered;	// Added to JPEG library to report errors.






#ifndef HUFFMAN_VERSION_1_0_NOT_SUPPORTED

//
// Diff
//      Create a running difference for the input data sent by the caller.
//
void Diff(HSTRING *Data, int Interval)
{
	unsigned char	BaseValue;
	unsigned char	Temp;
	int				i;
	int				k;


	if((Interval < 1) || (Interval > ((int) Data->Size)))
	{
		return;
	}


	for(k=0; k<Interval; ++k)
	{
		BaseValue = Data->Char[k];
		for(i=k+Interval; i<((int) Data->Size); i+=Interval)
		{
			Temp = Data->Char[i];
			//            Data->Char[i] -= BaseValue;
			Data->Char[i] = ((unsigned char) (Data->Char[i] - BaseValue));
			BaseValue = Temp;
		}
	}

	return;
}







//
// Undiff
//      Convert a running difference, sent by the caller, back to original data.
//
void Undiff(HSTRING *Data, int Interval)
{
	int   i;
	int   k;


	if((Interval < 1) || (Interval > ((int) Data->Size)))
	{
		return;
	}


	for(k=0; k<Interval; ++k)
	{
		for(i=k+Interval; i<((int) Data->Size); i+=Interval)
		{
			//            Data->Char[i] += Data->Char[i-Interval];
			Data->Char[i] = ((unsigned char) (Data->Char[i] + Data->Char[i-Interval]));
		}
	}


	return;
}








/************************** Start of AHUFF.C *************************
 *
 * This is the adaptive Huffman coding module used in Chapter 4.
 * Compile with BITIO.C, ERRHAND.C, and either MAIN-C.C or MAIN-E.C
 */


#define END_OF_HUFF_STREAM  256
#define ESCAPE              257
#define SYMBOL_COUNT        320		/* 258 */
#define NODE_TABLE_COUNT    ( ( SYMBOL_COUNT * 2 ) - 1 )
#define ROOT_NODE           0
#define MAX_WEIGHT          0x8000

/*
 * This data structure is all that is needed to maintain an adaptive
 * Huffman tree for both encoding and decoding.  The leaf array is a
 * set of indices into the nodes that indicate which node is the
 * parent of a symbol.  For example, to encode 'A', we would find the
 * leaf node by way of leaf[ 'A' ].  The next_free_node index is used
 * to tell which node is the next one in the array that can be used.
 * Since nodes are allocated when characters are read in for the first
 * time, this pointer keeps track of where we are in the node array.
 * Finally, the array of nodes is the actual Huffman tree.  The child
 * index is either an index pointing to a pair of children, or an
 * actual symbol value, depending on whether 'child_is_leaf' is true
 * or false.
 */

typedef struct HUFF_NODE
{
	unsigned int weight;
	int parent;
	int child_is_leaf;
	int child;
}
huff_node;

typedef struct huff_tree
{
	int leaf[ SYMBOL_COUNT + 20];
	int next_free_node;
	/*    struct node {
	        unsigned int weight;
	        int parent;
	        int child_is_leaf;
	        int child;
	    } nodes[ NODE_TABLE_COUNT + 20];*/
	huff_node    nodes[ NODE_TABLE_COUNT + 20 ];
}
HUFF_TREE;

/*
 * The Tree used in this program is a global structure.  Under other
 * circumstances it could just as well be a dynamically allocated
 * structure built when needed, since all routines here take a TREE
 * pointer as an argument.
 */

HUFF_TREE HuffTree;

/*
 * Function prototypes for both ANSI C compilers and their K&R brethren.
 */

void InitializeHuffmanTree( HUFF_TREE *tree );
void EncodeSymbol( HUFF_TREE *tree, unsigned int c, BITSTRING);
int DecodeSymbol( HUFF_TREE *tree, BITSTRING);
void UpdateModel( HUFF_TREE *tree, int c );
void RebuildHuffmanTree( HUFF_TREE *tree );
void swap_nodes( HUFF_TREE *tree, int i, int j );
void add_new_node( HUFF_TREE *tree, int c );


/*
 * When performing adaptive compression, the Huffman tree starts out
 * very nearly empty.  The only two symbols present initially are the
 * ESCAPE symbol and the END_OF_STREAM symbol.  The ESCAPE symbol has to
 * be included so we can tell the expansion prog that we are transmitting a
 * previously unseen symbol.  The END_OF_STREAM symbol is here because
 * it is greater than eight bits, and our ESCAPE sequence only allows for
 * eight bit symbols following the ESCAPE code.
 *
 * In addition to setting up the root node and its two children, this
 * routine also initializes the leaf array.  The ESCAPE and END_OF_STREAM
 * leaf elements are the only ones initially defined, the rest of the leaf
 * elements are set to -1 to show that they aren't present in the
 * Huffman tree yet.
 */

void InitializeHuffmanTree(HUFF_TREE *tree )
{
	int i;

	tree->nodes[ ROOT_NODE ].child             = ROOT_NODE + 1;
	tree->nodes[ ROOT_NODE ].child_is_leaf     = FALSE;
	tree->nodes[ ROOT_NODE ].weight            = 2;
	tree->nodes[ ROOT_NODE ].parent            = -1;

	tree->nodes[ ROOT_NODE + 1 ].child         = END_OF_HUFF_STREAM;
	tree->nodes[ ROOT_NODE + 1 ].child_is_leaf = TRUE;
	tree->nodes[ ROOT_NODE + 1 ].weight        = 1;
	tree->nodes[ ROOT_NODE + 1 ].parent        = ROOT_NODE;
	tree->leaf[ END_OF_HUFF_STREAM ]           = ROOT_NODE + 1;

	tree->nodes[ ROOT_NODE + 2 ].child         = ESCAPE;
	tree->nodes[ ROOT_NODE + 2 ].child_is_leaf = TRUE;
	tree->nodes[ ROOT_NODE + 2 ].weight        = 1;
	tree->nodes[ ROOT_NODE + 2 ].parent        = ROOT_NODE;
	tree->leaf[ ESCAPE ]                       = ROOT_NODE + 2;

	tree->next_free_node                       = ROOT_NODE + 3;

	for ( i = 0 ; i < END_OF_HUFF_STREAM ; i++ )
		tree->leaf[ i ] = -1;
}

/*
 * This routine is responsible for taking a symbol, and converting
 * it into the sequence of bits dictated by the Huffman tree.  The
 * only complication is that we are working are way up from the leaf
 * to the root, and hence are getting the bits in reverse order.  This
 * means we have to rack up the bits in an integer and then send them
 * out after they are all accumulated.  In this version of the program,
 * we keep our codes in a long integer, so the maximum count is set
 * to an arbitray limit of 0x8000.  It could be set as high as 65535
 * if desired.
 */

void EncodeSymbol(HUFF_TREE *tree, unsigned int c, BITSTRING *BitString)
{
	int   code;
	int   current_bit;
	int   code_size;
	int current_node;

	code = 0;
	current_bit = 1;
	code_size = 0;
	current_node = tree->leaf[ c ];
	if ( current_node == -1 )
		current_node = tree->leaf[ ESCAPE ];
	while ( current_node != ROOT_NODE )
	{
		if ( ( current_node & 1 ) == 0 )
			code |= current_bit;
		current_bit <<= 1;
		code_size++;
		current_node = tree->nodes[ current_node ].parent;
	};
	BitString->AppendBits(code, code_size);
	if ( tree->leaf[ c ] == -1 )
	{
		BitString->AppendBits(((int) c), 8);
		add_new_node( tree, c );
	}
}

/*
 * Decoding symbols is easy.  We start at the root node, then go down
 * the tree until we reach a leaf.  At each node, we decide which
 * child to take based on the next input bit.  After getting to the
 * leaf, we check to see if we read in the ESCAPE code.  If we did,
 * it means that the next symbol is going to come through in the next
 * eight bits, unencoded.  If that is the case, we read it in here,
 * and add the new symbol to the table.
 */

int DecodeSymbol(HUFF_TREE *tree, BITSTRING *BitString)
{
	int current_node;
	int c;

	current_node = ROOT_NODE;
	while ((!BitString->EndOfBitString()) && ( !tree->nodes[ current_node ].child_is_leaf ))
	{
		current_node = tree->nodes[ current_node ].child;
		current_node += ((int) BitString->CopyNextBits(1));
	}
	c = tree->nodes[ current_node ].child;
	if ( c == ESCAPE )
	{
		c = ((int) BitString->CopyNextBits(8));
		add_new_node( tree, c );
	}
	return( c );
}

/*
 * UpdateModel is called to increment the count for a given symbol.
 * After incrementing the symbol, this code has to work its way up
 * through the parent nodes, incrementing each one of them.  That is
 * the easy part.  The hard part is that after incrementing each
 * parent node, we have to check to see if it is now out of the proper
 * order.  If it is, it has to be moved up the tree into its proper
 * place.
 */
void UpdateModel(HUFF_TREE *tree, int c )
{
	int current_node;
	int new_node;

	if ( tree->nodes[ ROOT_NODE].weight == MAX_WEIGHT )
		RebuildHuffmanTree( tree );
	current_node = tree->leaf[ c ];
	while ( current_node != -1 )
	{
		tree->nodes[ current_node ].weight++;
		for ( new_node = current_node ; new_node > ROOT_NODE ; new_node-- )
			if ( tree->nodes[ new_node - 1 ].weight >=
			        tree->nodes[ current_node ].weight )
				break;
		if ( current_node != new_node )
		{
			swap_nodes( tree, current_node, new_node );
			current_node = new_node;
		}
		current_node = tree->nodes[ current_node ].parent;
	}
}

/*
 * Rebuilding the tree takes place when the counts have gone too
 * high.  From a simple point of view, rebuilding the tree just means that
 * we divide every count by two.  Unfortunately, due to truncation effects,
 * this means that the tree's shape might change.  Some nodes might move
 * up due to cumulative increases, while others may move down.
 */




void RebuildHuffmanTree(HUFF_TREE *tree)
{
	int i;
	int j;
	int k;
	unsigned int weight;

	/*
	 * To start rebuilding the table,  I collect all the leaves of the Huffman
	 * tree and put them in the end of the tree.  While I am doing that, I
	 * scale the counts down by a factor of 2.
	 */
	j = tree->next_free_node - 1;
	for ( i = j ; i >= ROOT_NODE ; i-- )
	{
		if ( tree->nodes[ i ].child_is_leaf )
		{
			tree->nodes[ j ] = tree->nodes[ i ];
			tree->nodes[ j ].weight = ( tree->nodes[ j ].weight + 1 ) / 2;
			j--;
		}
	}

	/*
	 * At this point, j points to the first free node.  I now have all the
	 * leaves defined, and need to start building the higher nodes on the
	 * tree. I will start adding the new internal nodes at j.  Every time
	 * I add a new internal node to the top of the tree, I have to check to
	 * see where it really belongs in the tree.  It might stay at the top,
	 * but there is a good chance I might have to move it back down.  If it
	 * does have to go down, I use the memmove() function to scoot everyone
	 * bigger up by one node.  Note that memmove() may have to be change
	 * to memcpy() on some UNIX systems.  The parameters are unchanged, as
	 * memmove and  memcpy have the same set of parameters.
	 */
	for ( i = tree->next_free_node - 2 ; j >= ROOT_NODE ; i -= 2, j-- )
	{
		k = i + 1;
		tree->nodes[ j ].weight = tree->nodes[ i ].weight +
		                          tree->nodes[ k ].weight;
		weight = tree->nodes[ j ].weight;
		tree->nodes[ j ].child_is_leaf = FALSE;
		for ( k = j + 1 ; weight < tree->nodes[ k ].weight ; k++ )
			;
		k--;
		memmove( &tree->nodes[ j ], &tree->nodes[ j + 1 ],
		         ( k - j ) * sizeof( huff_node ) );
		tree->nodes[ k ].weight = weight;
		tree->nodes[ k ].child = i;
		tree->nodes[ k ].child_is_leaf = FALSE;
	}
	/*
	 * The final step in tree reconstruction is to go through and set up
	 * all of the leaf and parent members.  This can be safely done now
	 * that every node is in its final position in the tree.
	 */
	for ( i = tree->next_free_node - 1 ; i >= ROOT_NODE ; i-- )
	{
		if ( tree->nodes[ i ].child_is_leaf )
		{
			k = tree->nodes[ i ].child;
			tree->leaf[ k ] = i;
		}
		else
		{
			k = tree->nodes[ i ].child;
			tree->nodes[ k ].parent = tree->nodes[ k + 1 ].parent = i;
		}
	}
}

/*
 * Swapping nodes takes place when a node has grown too big for its
 * spot in the tree.  When swapping nodes i and j, we rearrange the
 * tree by exchanging the children under i with the children under j.
 */

void swap_nodes(HUFF_TREE *tree, int i, int j)
{
	huff_node temp;

	if ( tree->nodes[ i ].child_is_leaf )
		tree->leaf[ tree->nodes[ i ].child ] = j;
	else
	{
		tree->nodes[ tree->nodes[ i ].child ].parent = j;
		tree->nodes[ tree->nodes[ i ].child + 1 ].parent = j;
	}
	if ( tree->nodes[ j ].child_is_leaf )
		tree->leaf[ tree->nodes[ j ].child ] = i;
	else
	{
		tree->nodes[ tree->nodes[ j ].child ].parent = i;
		tree->nodes[ tree->nodes[ j ].child + 1 ].parent = i;
	}
	temp = tree->nodes[ i ];
	tree->nodes[ i ] = tree->nodes[ j ];
	tree->nodes[ i ].parent = temp.parent;
	temp.parent = tree->nodes[ j ].parent;
	tree->nodes[ j ] = temp;
}

/*
 * Adding a new node to the tree is pretty simple.  It is just a matter
 * of splitting the lightest-weight node in the tree, which is the highest
 * valued node.  We split it off into two new nodes, one of which is the
 * one being added to the tree.  We assign the new node a weight of 0,
 * so the tree doesn't have to be adjusted.  It will be updated later when
 * the normal update process occurs.  Note that this code assumes that
 * the lightest node has a leaf as a child.  If this is not the case,
 * the tree would be broken.
 */
void add_new_node(HUFF_TREE *tree, int c)
{
	int lightest_node;
	int new_node;
	int zero_weight_node;

	lightest_node = tree->next_free_node - 1;
	new_node = tree->next_free_node;
	zero_weight_node = tree->next_free_node + 1;
	tree->next_free_node += 2;

	tree->nodes[ new_node ] = tree->nodes[ lightest_node ];
	tree->nodes[ new_node ].parent = lightest_node;
	tree->leaf[ tree->nodes[ new_node ].child ] = new_node;

	tree->nodes[ lightest_node ].child         = new_node;
	tree->nodes[ lightest_node ].child_is_leaf = FALSE;

	tree->nodes[ zero_weight_node ].child           = c;
	tree->nodes[ zero_weight_node ].child_is_leaf   = TRUE;
	tree->nodes[ zero_weight_node ].weight          = 0;
	tree->nodes[ zero_weight_node ].parent          = lightest_node;
	tree->leaf[ c ] = zero_weight_node;
}
/************************** End of AHUFF.C ****************************/



#endif // #ifndef HUFFMAN_VERSION_1_0_NOT_SUPPORTED





bool HuffmanEncode(HSTRING *Data, int Interval)
{
#ifdef HUFFMAN_VERSION_1_0_NOT_SUPPORTED
	return false;

#else

	BITSTRING       *BitString;
	int	UncompressedSize;
	bool    ReturnStatus;
	int	i;
	int     c;


	// Store the uncompressed size
	UncompressedSize = Data->Size;

	RESET_WATCHDOG_TIMER();

	// Create space for the bit string.
	BitString = new BITSTRING(((unsigned long) (Data->Size * 5 / 4)));
	BitString->ResetBitPointer();

	// Compute running difference for the input data.
	Diff(Data, Interval);

	InitializeHuffmanTree( &HuffTree );

	i = 0;
	while (i < ((int) Data->Size))
	{
		RESET_WATCHDOG_TIMER();
		c = ((unsigned int) Data->Char[i]);
		EncodeSymbol( &HuffTree, c, BitString);
		UpdateModel( &HuffTree, c);
		++i;
	}
	EncodeSymbol( &HuffTree, END_OF_HUFF_STREAM, BitString);


	// Copy the original (uncompressed) size back to the beginning of the Data stream.
	Data->Size = 0;
	ReturnStatus = Data->AppendNumber(UncompressedSize, sizeof(int));

	// Copy the interval to the Data stream.
	ReturnStatus = Data->AppendNumber(Interval, sizeof(int));

	// Copy the Huffman encoded bit stream to the Data stream.
	ReturnStatus = Data->Append(BitString->Char, BitString->Size);

	delete BitString;

	RESET_WATCHDOG_TIMER();

	return(ReturnStatus);

#endif
}
//---------------------------------------------------------------------------







//
// HuffmanDecode
//      Decompress data, sent by the caller, via adaptive Huffman.
//
bool HuffmanDecode(HSTRING *Data)
{
#ifdef HUFFMAN_VERSION_1_0_NOT_SUPPORTED
	return false;

#else

	int	UncompressedSize;
	int	Interval;




	// Create space for the bit string.
	BITSTRING *BitString = new BITSTRING(Data->Size);
	if (!BitString)
		return false;
	// Copy the input data string
	BitString->Copy(&(Data->Char[sizeof(UncompressedSize) + sizeof(Interval)]),
	                Data->Size - sizeof(UncompressedSize) - sizeof(Interval));
	BitString->Copy(&(Data->Char[8]), Data->Size - 8);
	BitString->ResetBitPointer();

	RESET_WATCHDOG_TIMER();

	// Get the size of the uncompressed data.
	Data->CopyNumberFromString(0, sizeof(UncompressedSize), &UncompressedSize);

	// Get the difference interval of the uncompressed data.
	Data->CopyNumberFromString(sizeof(UncompressedSize), sizeof(Interval), &Interval);


	Data->Size = 0;

	InitializeHuffmanTree( &HuffTree );
	bool    ReturnStatus=true;
	int  c;
	while (ReturnStatus && (!BitString->EndOfBitString()) && ((c = DecodeSymbol( &HuffTree, BitString) ) != END_OF_HUFF_STREAM ))
	{
		RESET_WATCHDOG_TIMER();
		ReturnStatus = Data->AppendNumber(c, 1);
		UpdateModel( &HuffTree, c );
	}

	Undiff(Data, Interval);

	RESET_WATCHDOG_TIMER();

	delete BitString;

	return(ReturnStatus);

#endif
}
//---------------------------------------------------------------------------



























#ifndef LEMPEL_ZIV_VERSION_1_0_NOT_SUPPORTED





/************************** Start of LZSS.C *************************
 *
 * This is the LZSS module, which implements an LZ77 style compression
 * algorithm.  As iplemented here it uses a 12 bit index into the sliding
 * window, and a 4 bit length, which is adjusted to reflect phrase lengths
 * of between 2 and 17 bytes.
 */

/*
 * Various constants used to define the compression parameters.  The
 * INDEX_BIT_COUNT tells how many bits we allocate to indices into the
 * text window.  This directly determines the WINDOW_SIZE.  The
 * LENGTH_BIT_COUNT tells how many bits we allocate for the length of
 * an encode phrase. This determines the size of the look ahead buffer.
 * The TREE_ROOT is a special node in the tree that always points to
 * the root node of the binary phrase tree.  END_OF_STREAM is a special
 * index used to flag the fact that the file has been completely
 * encoded, and there is no more data.  LZ_UNUSED is the null index for
 * the tree. MOD_WINDOW() is a macro used to perform arithmetic on tree
 * indices.
 *
 */

#define INDEX_BIT_COUNT      12
#define LENGTH_BIT_COUNT     4
#define WINDOW_SIZE          ( 1 << INDEX_BIT_COUNT )
#define RAW_LOOK_AHEAD_SIZE  ( 1 << LENGTH_BIT_COUNT )
#define BREAK_EVEN           ( ( 1 + INDEX_BIT_COUNT + LENGTH_BIT_COUNT ) / 9 )
#define LOOK_AHEAD_SIZE      ( RAW_LOOK_AHEAD_SIZE + BREAK_EVEN )
#define TREE_ROOT            WINDOW_SIZE
#define END_OF_LZ_STREAM     0
#define LZ_UNUSED			 0
#define MOD_WINDOW( a )      ( ( a ) & ( WINDOW_SIZE - 1 ) )

/*
 * These are the two global data structures used in this program.
 * The window[] array is exactly that, the window of previously seen
 * text, as well as the current look ahead text.  The tree[] structure
 * contains the binary tree of all of the strings in the window sorted
 * in order.
*/

unsigned char window[ WINDOW_SIZE ];

struct
{
	int parent;
	int smaller_child;
	int larger_child;
}
lz_tree[ WINDOW_SIZE + 1 ];

/*
 * Function prototypes for both ANSI C compilers and their K&R brethren.
 */

void InitLZTree( int r );
void ContractNode( int old_node, int new_node );
void ReplaceNode( int old_node, int new_node );
int FindNextNode( int node );
void DeleteString( int p );
int AddString( int new_node, int *match_position );

/*
 * Since the tree is static data, it comes up with every node
 * initialized to 0, which is good, since 0 is the LZ_UNUSED code.
 * However, to make the tree really usable, a single phrase has to be
 * added to the tree so it has a root node.  That is done right here.
*/
void InitLZTree( int r )
{
	lz_tree[ TREE_ROOT ].larger_child = r;
	lz_tree[ r ].parent = TREE_ROOT;
	lz_tree[ r ].larger_child = LZ_UNUSED;
	lz_tree[ r ].smaller_child = LZ_UNUSED;
}

/*
 * This routine is used when a node is being deleted.  The link to
 * its descendant is broken by pulling the descendant in to overlay
 * the existing link.
 */
void ContractNode( int old_node, int new_node )
{
	lz_tree[ new_node ].parent = lz_tree[ old_node ].parent;
	if ( lz_tree[ lz_tree[ old_node ].parent ].larger_child == old_node )
		lz_tree[ lz_tree[ old_node ].parent ].larger_child = new_node;
	else
		lz_tree[ lz_tree[ old_node ].parent ].smaller_child = new_node;
	lz_tree[ old_node ].parent = LZ_UNUSED;
}

/*
 * This routine is also used when a node is being deleted.  However,
 * in this case, it is being replaced by a node that was not previously
 * in the tree.
 */
void ReplaceNode( int old_node, int new_node )
{
	int parent;

	parent = lz_tree[ old_node ].parent;
	if ( lz_tree[ parent ].smaller_child == old_node )
		lz_tree[ parent ].smaller_child = new_node;
	else
		lz_tree[ parent ].larger_child = new_node;
	lz_tree[ new_node ] = lz_tree[ old_node ];
	lz_tree[ lz_tree[ new_node ].smaller_child ].parent = new_node;
	lz_tree[ lz_tree[ new_node ].larger_child ].parent = new_node;
	lz_tree[ old_node ].parent = LZ_UNUSED;
}

/*
 * This routine is used to find the next smallest node after the node
 * argument.  It assumes that the node has a smaller child.  We find
 * the next smallest child by going to the smaller_child node, then
 * going to the end of the larger_child descendant chain.
*/
int FindNextNode( int node )
{
	int next;

	next = lz_tree[ node ].smaller_child;
	while ( lz_tree[ next ].larger_child != LZ_UNUSED )
		next = lz_tree[ next ].larger_child;
	return( next );
}

/*
 * This routine performs the classic binary tree deletion algorithm.
 * If the node to be deleted has a null link in either direction, we
 * just pull the non-null link up one to replace the existing link.
 * If both links exist, we instead delete the next link in order, which
 * is guaranteed to have a null link, then replace the node to be deleted
 * with the next link.
 */
void DeleteString( int p )
{
	int  replacement;

	if ( lz_tree[ p ].parent == LZ_UNUSED )
		return;
	if ( lz_tree[ p ].larger_child == LZ_UNUSED )
		ContractNode( p, lz_tree[ p ].smaller_child );
	else if ( lz_tree[ p ].smaller_child == LZ_UNUSED )
		ContractNode( p, lz_tree[ p ].larger_child );
	else
	{
		replacement = FindNextNode( p );
		DeleteString( replacement );
		ReplaceNode( p, replacement );
	}
}

/*
 * This where most of the work done by the encoder takes place.  This
 * routine is responsible for adding the new node to the binary tree.
 * It also has to find the best match among all the existing nodes in
 * the tree, and return that to the calling routine.  To make matters
 * even more complicated, if the new_node has a duplicate in the tree,
 * the old_node is deleted, for reasons of efficiency.
 */

int AddString( int new_node, int *match_position )
{
	int i;
	int test_node;
	int delta;
	int match_length;
	int *child;

	if ( new_node == END_OF_LZ_STREAM )
		return( 0 );
	test_node = lz_tree[ TREE_ROOT ].larger_child;
	match_length = 0;
	for ( ; ; )
	{
		for ( i = 0 ; i < LOOK_AHEAD_SIZE ; i++ )
		{
			delta = window[ MOD_WINDOW( new_node + i ) ] -
			        window[ MOD_WINDOW( test_node + i ) ];
			if ( delta != 0 )
				break;
		}
		if ( i >= match_length )
		{
			match_length = i;
			*match_position = test_node;
			if ( match_length >= LOOK_AHEAD_SIZE )
			{
				ReplaceNode( test_node, new_node );
				return( match_length );
			}
		}
		if ( delta >= 0 )
			child = &lz_tree[ test_node ].larger_child;
		else
			child = &lz_tree[ test_node ].smaller_child;
		if ( *child == LZ_UNUSED )
		{
			*child = new_node;
			lz_tree[ new_node ].parent = test_node;
			lz_tree[ new_node ].larger_child = LZ_UNUSED;
			lz_tree[ new_node ].smaller_child = LZ_UNUSED;
			return( match_length );
		}
		test_node = *child;
	}
}


#endif // #ifndef LEMPEL_ZIV_VERSION_1_0_NOT_SUPPORTED



/*
 * This is the compression routine.  It has to first load up the look
 * ahead buffer, then go into the main compression loop.  The main loop
 * decides whether to output a single character or an index/length
 * token that defines a phrase.  Once the character or phrase has been
 * sent out, another loop has to run.  The second loop reads in new
 * characters, deletes the strings that are overwritten by the new
 * character, then adds the strings that are created by the new
 * character.
 */

bool LZEncode(HSTRING *Data)
{
#ifdef LEMPEL_ZIV_VERSION_1_0_NOT_SUPPORTED
	return	false;

#else

	BITSTRING       *BitString;
	int	UncompressedSize;
	bool    ReturnStatus;
	int i;
	int c;
	int look_ahead_bytes;
	int current_position;
	int replace_count;
	int match_length;
	int match_position;
	int NextDataItem;


	RESET_WATCHDOG_TIMER();

	// Store the uncompressed size
	UncompressedSize = Data->Size;

	// Create space for the bit string.
	BitString = new BITSTRING(((unsigned long) (Data->Size * 5 / 4)));
	BitString->ResetBitPointer();


	// Zero out the sliding window and parsing tree.  If this isn't done, results are
	// unpredictable.
	for(i=0; i<WINDOW_SIZE; ++i)
	{
		window[i] = 0;
	}
	for(i=0; i<(WINDOW_SIZE+1); ++i)
	{
		lz_tree[i].parent = 0;
		lz_tree[i].smaller_child = 0;
		lz_tree[i].larger_child = 0;
	}

	current_position = 1;
	NextDataItem = 0;
	for ( i = 0 ; i < LOOK_AHEAD_SIZE ; i++ )
	{
		if(NextDataItem >= ((int) UncompressedSize))
		{
			break;
		}
		else
		{
			c = ((unsigned int) Data->Char[NextDataItem++]);
		}
		window[ current_position + i ] = (unsigned char) c;
	}

	RESET_WATCHDOG_TIMER();

	look_ahead_bytes = i;
	InitLZTree( current_position );
	match_length = 0;
	match_position = 0;
	while ( look_ahead_bytes > 0 )
	{
		RESET_WATCHDOG_TIMER();
		if ( match_length > look_ahead_bytes )
			match_length = look_ahead_bytes;
		if ( match_length <= BREAK_EVEN )
		{
			replace_count = 1;
			BitString->AppendBits(1, 1);
			BitString->AppendBits((unsigned long) window[ current_position ], 8 );
		}
		else
		{
			BitString->AppendBits(0, 1);
			BitString->AppendBits((unsigned long) match_position, INDEX_BIT_COUNT );
			BitString->AppendBits((unsigned long) ( match_length - ( BREAK_EVEN + 1 ) ), LENGTH_BIT_COUNT );
			replace_count = match_length;
		}
		for ( i = 0 ; i < replace_count ; i++ )
		{
			DeleteString( MOD_WINDOW( current_position + LOOK_AHEAD_SIZE ) );
			if(NextDataItem >= ((int) UncompressedSize))
			{
				look_ahead_bytes--;
			}
			else
			{
				c = ((unsigned int) Data->Char[NextDataItem++]);
				window[ MOD_WINDOW( current_position + LOOK_AHEAD_SIZE ) ]
				= (unsigned char) c;
			}
			current_position = MOD_WINDOW( current_position + 1 );
			if ( look_ahead_bytes )
				match_length = AddString( current_position, &match_position );
		}
	};
	BitString->AppendBits(0, 1);
	BitString->AppendBits((unsigned long) END_OF_LZ_STREAM, INDEX_BIT_COUNT );


	// Copy the original (uncompressed) size back to the beginning of the Data stream.
	Data->Size = 0;
	ReturnStatus = Data->AppendNumber(UncompressedSize, sizeof(UncompressedSize));

	// Copy the Lempel-Ziv encoded bit stream to the Data stream.
	ReturnStatus = Data->Append(BitString->Char, BitString->Size);

	delete BitString;

	return(ReturnStatus);

#endif
}





/*
 * This is the expansion routine for the LZSS algorithm.  All it has
 * to do is read in flag bits, decide whether to read in a character or
 * a index/length pair, and take the appropriate action.
*/

bool LZDecode(HSTRING *Data)
{
#ifdef LEMPEL_ZIV_VERSION_1_0_NOT_SUPPORTED
	return	false;

#else

	BITSTRING       *BitString;
	int	UncompressedSize;
	bool    ReturnStatus;
	int i;
	int current_position;
	int c;
	int match_length;
	int match_position;



	// Create space for the bit string.
	BitString = new BITSTRING(Data->Size);

	// Copy the input data string
	BitString->Copy(&(Data->Char[sizeof(UncompressedSize)]), Data->Size - sizeof(UncompressedSize));
	BitString->ResetBitPointer();


	// Get the size of the uncompressed data.
	Data->CopyNumberFromString(0, sizeof(UncompressedSize), &UncompressedSize);


	Data->Size = 0;


	current_position = 1;
	for ( ; !BitString->EndOfBitString(); )
	{
		RESET_WATCHDOG_TIMER();
		if (BitString->CopyNextBits(1))
		{
			c = ((int) BitString->CopyNextBits(8));
			ReturnStatus = Data->AppendNumber(c, 1);
			window[ current_position ] = (unsigned char) c;
			current_position = MOD_WINDOW( current_position + 1 );
		}
		else
		{
			match_position = ((int) BitString->CopyNextBits(INDEX_BIT_COUNT));
			if ( match_position == END_OF_LZ_STREAM )
				break;
			match_length = ((int) BitString->CopyNextBits(LENGTH_BIT_COUNT ));
			match_length += BREAK_EVEN;
			for ( i = 0 ; i <= match_length ; i++ )
			{
				c = window[ MOD_WINDOW( match_position + i ) ];
				ReturnStatus = Data->AppendNumber(c, 1);
				window[ current_position ] = (unsigned char) c;
				current_position = MOD_WINDOW( current_position + 1 );
			}
		}
	}


	delete BitString;

	return(ReturnStatus);

#endif
}

/************************** End of LZSS.C *************************/











// ******************************************************************************
// *
// *  Name:        JPEGLossyEncode()
// *
// *  Description: Builds a JPEG baseline lossy compressed file in memory.
// *
// *  Parameters:  HSTRING	*Data;		(I/O)	// Buffer with data in & which will
// *											// contain file data on return.
// *               int		dwQfactor;	(I)		// Quality factor.
// *
// *  Returns:     true if success, false otherwise
// *
// ******************************************************************************
extern "C" JOCTET	*puchOutBuff;
extern "C" int		puchOutBuffSize;
bool JPEGLossyEncode(HSTRING *Data, int XDimension, int YDimension, int QualityFactor, int CompressionMode,
                     int SmoothingFactor, int colored)
{
	bool							ReturnStatus = true;		// No errors yet.
	struct	jpeg_error_mgr			JPEGError;					// JPEG library error structure
	struct	jpeg_compress_struct	JPEGCompressionInfo;		// JPEG library compression info struct.
	JSAMPROW						row_pointer[1];				// Used to point to source on JPEG call.
	int								n;



	FatalJPEGErrorEncountered = 0;


	// Check compression mode.
	if(CompressionMode == JPEG_LOSSY_1_0_FP)
	{
		CompressionMode = JPEG_LOSSY_1_0;
	}

	if((CompressionMode != JPEG_LOSSY_1_0) && (CompressionMode != JPEG_LOSSY_FAST_1_0))
	{
		return(false);
	}


	// Compute output buffer size as image size + 30 percent if quality factor is below 95, and set it to image size + 50 percent
	// otherwise.
	if( QualityFactor >= 95 )
	{
		puchOutBuffSize = ( XDimension * YDimension * 150 ) / 100;
	}
	else
	{
		puchOutBuffSize = ( XDimension * YDimension * 130 ) / 100;
	}

	puchOutBuff = 0;		// Set to zero so we'll know if it was initialized properly.
	puchOutBuff = new JOCTET[ puchOutBuffSize ];

	if( !puchOutBuff )		// If allocation failed, WOOF!
	{
		return false;
	}


	// Range check quality factor.
	if(QualityFactor < 0)
	{
		QualityFactor = 0;
	}
	if(QualityFactor > 100)
	{
		QualityFactor = 100;
	}

	// Range check smoothing factor.
	if(SmoothingFactor < 0)
	{
		SmoothingFactor = 0;
	}
	if(SmoothingFactor > 100)
	{
		SmoothingFactor = 100;
	}

	// Now let's convert our quality factor to JPEG library quality factor, which is inverted.
	//	QualityFactor = 100 - QualityFactor;


	// Setup error structure
	JPEGCompressionInfo.err = ::jpeg_std_error(&JPEGError);
	::jpeg_create_compress(&JPEGCompressionInfo);

	// Setup image description
	JPEGCompressionInfo.image_width = XDimension;
	JPEGCompressionInfo.image_height = YDimension;

	if(colored)   
	{
	JPEGCompressionInfo.input_components = 3;				// # of color components per pixel   
	JPEGCompressionInfo.in_color_space = JCS_RGB;       //   JCS_GRAYSCALE
	}
	else
	{
	JPEGCompressionInfo.input_components = 1;				// # of color components per pixel   
	JPEGCompressionInfo.in_color_space = JCS_GRAYSCALE;       //   JCS_GRAYSCALE
	}

	// Initialize compressor.
	::jpeg_set_defaults( &JPEGCompressionInfo);
	::jpeg_set_quality( &JPEGCompressionInfo, QualityFactor, TRUE );
	//	::jpeg_set_linear_quality( &JPEGCompressionInfo, QualityFactor, TRUE );

	JPEGCompressionInfo.smoothing_factor = SmoothingFactor;

	/* Now that we know input colorspace, fix colorspace-dependent defaults */
	//	::jpeg_default_colorspace(&JPEGCompressionInfo);

	::jpeg_stdio_dest(&JPEGCompressionInfo,NULL);

	if(CompressionMode == JPEG_LOSSY_1_0)
	{
		JPEGCompressionInfo.dct_method = JDCT_ISLOW;
	}
	else
	{
		JPEGCompressionInfo.dct_method = JDCT_IFAST;
	}

	RESET_WATCHDOG_TIMER();

	::jpeg_start_compress(&JPEGCompressionInfo, TRUE);

	// Compress the data.
	int		StartOfNextRow = 0;
	for(n = 0;    (n < YDimension) && (!FatalJPEGErrorEncountered);    n++)
	{
		RESET_WATCHDOG_TIMER();
		row_pointer[0] = &(Data->Char[StartOfNextRow]);
		::jpeg_write_scanlines( &JPEGCompressionInfo, row_pointer, 1);
		StartOfNextRow += XDimension*(JPEGCompressionInfo.input_components);
	}

	// Close out the compression. (Let compressor know this image done).
	::jpeg_finish_compress(&JPEGCompressionInfo);

	// Reallocate the data array sent by the caller if we need to.
	if((!FatalJPEGErrorEncountered) && (jpeg_get_compressed_buffer_len() > Data->MaximumLength()))
	{
		Data->Reallocate(((unsigned long) jpeg_get_compressed_buffer_len()));
	}

	RESET_WATCHDOG_TIMER();

	// If there is enough space for the compressed data, dump it, else WOOF.
	if((!FatalJPEGErrorEncountered) && (jpeg_get_compressed_buffer_len() <= Data->MaximumLength()))
	{
		Data->Size = jpeg_get_compressed_buffer(Data->Char, Data->MaximumLength());
	}
	else
	{
		ReturnStatus = false;
	}

	// Clean up allocated buffers.
	::jpeg_destroy_compress(&JPEGCompressionInfo);

	RESET_WATCHDOG_TIMER();

	if((ReturnStatus) && (FatalJPEGErrorEncountered))
	{
		ReturnStatus = false;
	}

	delete [] puchOutBuff;
	return(ReturnStatus);
} // JPEGLossyEncode









// ******************************************************************************
// *
// *  Name:        JPEGLossyDecode()
// *
// *  Description: Decompress both JPEG lossy and JPEG lossless (Huffman) data.
// *				The decompressed data is returned in the same buffer as the
// *				compressed data passed in.
// *
// *  Parameters:  HSTRING	*Data;		(I/O)	// Buffer with data in & which will
// *											// contain file data on return.
// *
// *  Returns:     true if success, false otherwise
// *
// ******************************************************************************

bool JPEGLossyDecode(HSTRING *Data, int CompressionMode, int *XDimension, int *YDimension, bool BlockSmoothing)
{
#ifdef JPEG_DECOMPRESSION_NOT_SUPPORTED
	return false;

#else

	bool							ReturnStatus = true;		// No errors yet.
	struct	jpeg_error_mgr			JPEGError;					// JPEG library error structure
	struct	jpeg_decompress_struct	JPEGCompressionInfo;		// JPEG library decompression info struct.
	JSAMPROW						row_pointer[1];				// Used to point to destination on JPEG call.
	int								n;



	FatalJPEGErrorEncountered = false;


	// Check compression mode.
	if(CompressionMode == JPEG_LOSSY_1_0_FP)
	{
		CompressionMode = JPEG_LOSSY_1_0;
	}

	if((CompressionMode != JPEG_LOSSY_1_0) && (CompressionMode != JPEG_LOSSY_FAST_1_0))
	{
		return(false);
	}


	// Make a copy of the input buffer.
	HSTRING	InputData(Data, true);

	// Zero out original data in preparation for decompression.
	Data->Size = 0;
	*XDimension = 0;
	*YDimension = 0;


	RESET_WATCHDOG_TIMER();

	// Intialize decompressor
	JPEGCompressionInfo.err = ::jpeg_std_error(&JPEGError);
	::jpeg_create_decompress(&JPEGCompressionInfo);
	::jpeg_buff_src(&JPEGCompressionInfo, (JOCTET *)InputData.Char, (size_t)InputData.Size);


	// Extra JPEG header information from the buffer
	if((::jpeg_read_header( &JPEGCompressionInfo,TRUE )) && (!FatalJPEGErrorEncountered))
	{
		// Set the compression mode
		if(CompressionMode == JPEG_LOSSY_1_0)
		{
			JPEGCompressionInfo.dct_method = JDCT_ISLOW;
		}
		else
		{
			JPEGCompressionInfo.dct_method = JDCT_IFAST;
		}

		JPEGCompressionInfo.do_block_smoothing = BlockSmoothing;

		// Make sure we'll have enough space for the output.
		if(((unsigned int) Data->MaximumLength()) < (JPEGCompressionInfo.image_width * JPEGCompressionInfo.image_height))
		{
			Data->Reallocate(JPEGCompressionInfo.image_width * JPEGCompressionInfo.image_height);
		}

		RESET_WATCHDOG_TIMER();

		// If we still don't have enough space, WOOF, else get the image.
		if(((unsigned int) Data->MaximumLength()) < (JPEGCompressionInfo.image_width * JPEGCompressionInfo.image_height))
		{
			ReturnStatus = false;
		}

		else
		{
			// Call the routine to begin decompression.
			::jpeg_start_decompress(&JPEGCompressionInfo);

			// Get whole image decompressing 1 line at a time.
			for( n = 0;    (((unsigned int) n) < JPEGCompressionInfo.image_height) && (ReturnStatus) && (!FatalJPEGErrorEncountered); n++ )
			{
				RESET_WATCHDOG_TIMER();

				// Setup pointer used to pass output buffer.
				row_pointer[0] = &(Data->Char[Data->Size]);

				// Decompress 1 line.
				::jpeg_read_scanlines( &JPEGCompressionInfo, row_pointer, 1);

				// Increment byte count.
				Data->Size += JPEGCompressionInfo.image_width;
			}
		}
		::jpeg_finish_decompress( &JPEGCompressionInfo );

		// Save image size, width and height if successful.
		*XDimension = JPEGCompressionInfo.image_width;
		*YDimension = JPEGCompressionInfo.image_height;
	}

	else
	{
		// Failed to parse JPEG header.
		ReturnStatus = false;
	}

	RESET_WATCHDOG_TIMER();

	::jpeg_destroy_decompress(&JPEGCompressionInfo);


	/*
		switch(CompressionMode)
		{
			case JPEG_LOSSY_1_0_FP:
				InverseDCT( input_array, output_array, CTM, CTMT);
				break;
	 
			case JPEG_LOSSY_1_0:
			default:
				InverseDCT( input_array, output_array, CTMI, CTMTI);
				break;
		}
	*/

	if((ReturnStatus) && (FatalJPEGErrorEncountered))
	{
		ReturnStatus = false;
	}

	return(ReturnStatus);

#endif
} // JPEGLossyDecode







bool ZLibCompress(HSTRING *Data, int level, int strategy)
{
	z_stream stream;
	int err;

	HSTRING		DataCopy(Data, true);
	Data->Size = 0;
	Data->Reallocate((DataCopy.Size * 3) / 2);

	stream.next_in = (Bytef*) DataCopy.Char;
	stream.avail_in = (uInt) DataCopy.Size;
#ifdef MAXSEG_64K
	/* Check for source > 64K on 16-bit machine: */
	if ((uLong)stream.avail_in != DataCopy.Size)
		return false;
#endif

	stream.next_out = Data->Char;
	stream.avail_out = (uInt) Data->MaximumLength();
	if ((uLong)stream.avail_out != ((uInt) Data->MaximumLength()))
		return false;

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	err = deflateInit2(&stream, level, Z_DEFLATED, MAX_WBITS, DEF_MEM_LEVEL, strategy);
	if (err != Z_OK)
		return false;

	err = deflate(&stream, Z_FINISH);
	if (err != Z_STREAM_END)
	{
		deflateEnd(&stream);
		Data->Size = stream.total_out;
		return ((err == Z_OK) ? true : false);
	}
	Data->Size = stream.total_out;

	err = deflateEnd(&stream);
	return((err == Z_OK) ? true : false);
}







bool ZLibDecompress(HSTRING *Data, int UncompressedSize)
{
	z_stream stream;
	int err;
	bool	ExpandBufferIfNeeded;
	int		InitialBufferSize;

	HSTRING		DataCopy(Data, true);
	Data->Size = 0;

	if(UncompressedSize < 1)
	{
		InitialBufferSize = DataCopy.Size * 3;
		Data->Reallocate( InitialBufferSize );
		ExpandBufferIfNeeded = true;
	}
	else
	{
		InitialBufferSize = UncompressedSize;
		Data->Reallocate(UncompressedSize);
		ExpandBufferIfNeeded = false;
	}

	stream.next_in = (Bytef*) DataCopy.Char;
	stream.avail_in = (uInt) DataCopy.Size;
	/* Check for source > 64K on 16-bit machine: */
	if ((uLong)stream.avail_in != DataCopy.Size)
		return false;

	stream.next_out = Data->Char;
	stream.avail_out = (uInt) Data->MaximumLength();
	if ((uLong)stream.avail_out != ((uInt) Data->MaximumLength()))
		return false;

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;

	err = inflateInit(&stream);
	if (err != Z_OK)
		return false;


	do
	{
		err = inflate(&stream, Z_SYNC_FLUSH);

		if( err == Z_OK )
		{
			if( ExpandBufferIfNeeded )
			{
				Data->Size = stream.total_out;
				if( Data->Reallocate( Data->MaximumLength() + InitialBufferSize ) )
				{
					stream.next_out = &Data->Char[ stream.total_out ];
					stream.avail_out = InitialBufferSize;
				}
				else
				{
					err = Z_BUF_ERROR;
				}
			}
			else
			{
				err = Z_BUF_ERROR;
			}
		}
	}
	while( err == Z_OK );


	if (err != Z_STREAM_END)
	{
		inflateEnd(&stream);
		Data->Size = stream.total_out;
		return ((err == Z_OK) ? true : false);
	}
	Data->Size = stream.total_out;

	err = inflateEnd(&stream);
	return((err == Z_OK) ? true : false);
}







bool LZOLibCompress(HSTRING *Data)
{
#ifdef LZO_NOT_SUPPORTED
	return	false;

#else

	bool	Status = true;		// Assume success until we know otherwise.


	if (lzo_init() != LZO_E_OK)
	{
		Status = false;
	}

	else
	{
		unsigned char		*Output = new unsigned char[ (Data->Size * 3) / 2 ];
		unsigned int		OutputLength;
		HSTRING		OriginalData( Data, true );

		//		lzo_align_t __LZO_MMODEL *wrkmem = new lzo_align_t __LZO_MMODEL [ ((LZO1X_1_MEM_COMPRESS) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) + 2];
		lzo_byte	*wrkmem = new lzo_byte[LZO1X_999_MEM_COMPRESS + 2];

		//		int	LZOCompressStatus = lzo1x_1_compress(Data->Char, Data->Size, Output, &OutputLength, wrkmem);
		int	LZOCompressStatus = lzo1x_999_compress(Data->Char, Data->Size, Output, &OutputLength, wrkmem);

		delete wrkmem;

		if (LZOCompressStatus != LZO_E_OK)
		{
			Status = false;
		}

		else
		{
			//			Data->Copy(Output, OutputLength);
			if(Data->MaximumLength() < (int) OutputLength)
			{
				Data->Reallocate(OutputLength + 2);
			}
			LZOCompressStatus = lzo1x_optimize(Output, OutputLength, Data->Char, &Data->Size, NULL);

			if (LZOCompressStatus != LZO_E_OK)
			{
				Status = false;
			}

			else
			{
				Data->Copy(Output, OutputLength);
			}
		}

		delete Output;
	}


	return Status;

#endif
}







bool LZOLibDecompress(HSTRING *Data)
{
#ifdef LZO_NOT_SUPPORTED
	return	false;

#else

	bool	Status = true;		// Assume success until we know otherwise.


	if (lzo_init() != LZO_E_OK)
	{
		Status = false;
	}

	else
	{
		unsigned char		*Output = new unsigned char[ (Data->Size * 5) / 2 ];
		unsigned int		OutputLength;
		HSTRING		OriginalData( Data, true );

		int	LZOCompressStatus = lzo1x_decompress(Data->Char, Data->Size, Output, &OutputLength, NULL);

		if (( LZOCompressStatus != LZO_E_OK ) && ( LZOCompressStatus != LZO_E_INPUT_NOT_CONSUMED ))
		{
			Status = false;
		}

		else
		{
			Data->Copy(Output, OutputLength);
		}

		delete Output;
	}

	return Status;

#endif
}






//
// CompressData (version #1, which takes only data and compression mode as arguments).
//      Compress data sent by the caller according to the mode specified by the caller.
//
bool CompressData(HSTRING *Data, int CompressionMode)
{
	if(CompressionMode == NO_COMPRESSION)
	{
		return(true);
	}
	else if(CompressionMode == HUFFMAN_VERSION_1_0)
	{
		return(HuffmanEncode(Data, 640));
	}
	else if(CompressionMode == LEMPEL_ZIV_VERSION_1_0)
	{
		return(LZEncode(Data));
	}
	else if(CompressionMode == HUFFMAN_VERSION_2_0)
	{
		return(ZLibCompress(Data, Z_BEST_COMPRESSION, Z_HUFFMAN_ONLY));
	}
	else if(CompressionMode == LEMPEL_ZIV_VERSION_2_0)
	{
		return(ZLibCompress(Data, Z_BEST_COMPRESSION, Z_DEFAULT_STRATEGY));
	}
	else if(CompressionMode == JPEG_LOSSY_1_0)
	{
		return(JPEGLossyEncode(Data, 480, 640, 80, JPEG_LOSSY_1_0, 0, 0));
	}
	else if(CompressionMode == JPEG_LOSSY_FAST_1_0)
	{
		return(JPEGLossyEncode(Data, 480, 640, 80, JPEG_LOSSY_FAST_1_0, 0, 0));
	}
	else if(CompressionMode == JPEG_LOSSY_1_0_FP)
	{
		return(JPEGLossyEncode(Data, 480, 640, 80, JPEG_LOSSY_1_0_FP, 0, 0));
	}
	else if(CompressionMode == LZO)
	{
		return(LZOLibCompress(Data));
	}
	else
	{
		return(false);
	}
}




//
// CompressData (version #2, which takes data, compression mode, and Huffman difference interval as arguments).
//      Compress data sent by the caller according to the mode specified by the caller.
//
bool CompressData(HSTRING *Data, int CompressionMode, int DifferenceInterval)
{
	if(CompressionMode == NO_COMPRESSION)
	{
		return(true);
	}
	else if(CompressionMode == HUFFMAN_VERSION_1_0)
	{
		return(HuffmanEncode(Data, DifferenceInterval));
	}
	else if(CompressionMode == LEMPEL_ZIV_VERSION_1_0)
	{
		return(LZEncode(Data));
	}
	else if(CompressionMode == HUFFMAN_VERSION_2_0)
	{
		return(ZLibCompress(Data, Z_BEST_COMPRESSION, Z_HUFFMAN_ONLY));
	}
	else if(CompressionMode == LEMPEL_ZIV_VERSION_2_0)
	{
		return(ZLibCompress(Data, Z_BEST_COMPRESSION, Z_DEFAULT_STRATEGY));
	}
	else if(CompressionMode == JPEG_LOSSY_1_0)
	{
		return(JPEGLossyEncode(Data, 480, 640, 80, JPEG_LOSSY_1_0, 0, 0));
	}
	else if(CompressionMode == JPEG_LOSSY_FAST_1_0)
	{
		return(JPEGLossyEncode(Data, 480, 640, 80, JPEG_LOSSY_FAST_1_0, 0, 0));
	}
	else if(CompressionMode == JPEG_LOSSY_1_0_FP)
	{
		return(JPEGLossyEncode(Data, 480, 640, 80, JPEG_LOSSY_1_0_FP, 0, 0));
	}
	else if(CompressionMode == LZO)
	{
		return(LZOLibCompress(Data));
	}
	else
	{
		return(false);
	}
}




//
// CompressData (version #3, which takes data, compression mode, Huffman difference interval, rows, columns,
//		JPEG quality factor, and JPEG compression smoothing factor as arguments).
//      Compress data sent by the caller according to the mode specified by the caller.
//
bool CompressData(HSTRING *Data, int CompressionMode, int DifferenceInterval, int XDimension, int YDimension,
                  int QualityFactor, int SmoothingFactor, int colored)
{
	if(CompressionMode == NO_COMPRESSION)
	{
		return(true);
	}
	else if(CompressionMode == HUFFMAN_VERSION_1_0)
	{
		return(HuffmanEncode(Data, DifferenceInterval));
	}
	else if(CompressionMode == LEMPEL_ZIV_VERSION_1_0)
	{
		return(LZEncode(Data));
	}
	else if(CompressionMode == HUFFMAN_VERSION_2_0)
	{
		return(ZLibCompress(Data, Z_BEST_COMPRESSION, Z_HUFFMAN_ONLY));
	}
	else if(CompressionMode == LEMPEL_ZIV_VERSION_2_0)
	{
		return(ZLibCompress(Data, Z_BEST_COMPRESSION, Z_DEFAULT_STRATEGY));
	}
	else if(CompressionMode == JPEG_LOSSY_1_0)
	{
		return(JPEGLossyEncode(Data, XDimension, YDimension, QualityFactor, JPEG_LOSSY_1_0, SmoothingFactor, colored));
	}
	else if(CompressionMode == JPEG_LOSSY_FAST_1_0)
	{
		return(JPEGLossyEncode(Data, XDimension, YDimension, QualityFactor, JPEG_LOSSY_FAST_1_0, SmoothingFactor, colored));
	}
	else if(CompressionMode == JPEG_LOSSY_1_0_FP)
	{
		return(JPEGLossyEncode(Data, XDimension, YDimension, QualityFactor, JPEG_LOSSY_1_0_FP, SmoothingFactor, colored));
	}
	else if(CompressionMode == LZO)
	{
		return(LZOLibCompress(Data));
	}
	else
	{
		return(false);
	}
}







//
// DecompressData (version #1, which returns image dimensions if applicable and takes JPEG decompression
//		block smoothing as an argument)
//      Decompress data sent by the caller according to the mode specified by the caller.
//
bool DecompressData(HSTRING *Data, int CompressionMode, int *XDimension, int *YDimension, bool BlockSmoothing,
                    int UncompressedSize)
{
	*XDimension = 0;
	*YDimension = 0;

	if(CompressionMode == NO_COMPRESSION)
	{
		return(true);
	}
	else if(CompressionMode == HUFFMAN_VERSION_1_0)
	{
		return(HuffmanDecode(Data));
	}
	else if(CompressionMode == LEMPEL_ZIV_VERSION_1_0)
	{
		return(LZDecode(Data));
	}
	else if(CompressionMode == HUFFMAN_VERSION_2_0)
	{
		return(ZLibDecompress(Data, UncompressedSize));
	}
	else if(CompressionMode == LEMPEL_ZIV_VERSION_2_0)
	{
		return(ZLibDecompress(Data, UncompressedSize));
	}
	else if(CompressionMode == JPEG_LOSSY_1_0)
	{
		return(JPEGLossyDecode(Data, JPEG_LOSSY_1_0, XDimension, YDimension, BlockSmoothing));
	}
	else if(CompressionMode == JPEG_LOSSY_FAST_1_0)
	{
		return(JPEGLossyDecode(Data, JPEG_LOSSY_FAST_1_0, XDimension, YDimension, BlockSmoothing));
	}
	else if(CompressionMode == JPEG_LOSSY_1_0_FP)
	{
		return(JPEGLossyDecode(Data, JPEG_LOSSY_1_0_FP, XDimension, YDimension, BlockSmoothing));
	}
	else if(CompressionMode == LZO)
	{
		return(LZOLibDecompress(Data));
	}
	else
	{
		return(false);
	}
}






//
// DecompressData (version #2, which takes JPEG decompression block smoothing as an argument)
//      Decompress data sent by the caller according to the mode specified by the caller.
//
bool DecompressData(HSTRING *Data, int CompressionMode, bool BlockSmoothing, int UncompressedSize)
{
	int		XDimension, YDimension;

	return(DecompressData(Data, CompressionMode, &XDimension, &YDimension, BlockSmoothing, UncompressedSize));
}









