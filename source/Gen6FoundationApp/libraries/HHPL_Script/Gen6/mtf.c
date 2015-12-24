#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mtf.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define MAX_ROWS            	1280
#define MAX_COLS            	1280
                            	
#define NUM_LINES_DEBUG     	4
#define MIN_LINES_REQUIRED  	5
#define REF_ZONE_OFFSET     	5
#define MIN_QA_REF_ZONE_WIDTH   20
#define MAX_QA_REF_ZONE_WIDTH   250
#define MIN_NUM_RULINGS         9 /* 5 blacks and 4 whites in-between */
#define MAX_PIXELS_PER_RULING   12
#define MIN_NUM_RULING_EDGES    MIN_NUM_RULINGS
#define MIN_RULING_DISTANCE     ( MIN_NUM_RULINGS * MAX_PIXELS_PER_RULING )


static int error = 0;


typedef struct {
    unsigned char  *pImage;
    int             SizeY;
    int             SizeX;
    int             Pitch;
} IMAGEINFO;


static IMAGEINFO ImageInfo;
static unsigned int MIN_DERIV_THRESHOLD = 15;


/*
static void dump(unsigned char *buffer, int size)
{
    int i, j;

    for (i = 0; i < size; i += 16) {
        if (!(i & 15))
            PRINTF("[0x%-2x]: ", i);
        for (j = 0; (j < 16) && ((i+j) < size); j++)
            PRINTF("0x%-2x ", buffer[i+j]);
        PRINTF("\n");
    }
}
*/


static int validate_horiz_scan_boundaries(int row, int col1, int col2, int scan_method)
{
    int     size_x = ImageInfo.SizeX;
    int     size_y = ImageInfo.SizeY;


    if (row < 0 ||
	row >= size_y ||
	col1 < 0 ||
	col1 >= size_x ||
	col2 < 0 ||
	col2 >= size_x)
    {
	error = -1;
	return (-1);
    }

    return 0;
}

/* Return: number of scanned elements if successful, or (-1) if error */
static int ScanHorizLine(int row,                  /* Input: scanning row */
			 int col1,                 /* Input: scan start column */
			 int col2,                 /* Input: scan end column */
			 int scan_method,          /* Input: scan method (currently not used) */
			 int scan_increment,       /* Input: scan resolution in pixels (can be >= 1) */
			 unsigned int *p_scan_data,/* Output: scanned data */
			 int max_elements,         /* Input: size of the scan_data buffer */
			 int *p_num_elements)      /* Output: number of elements returned in scan_data buffer */
{
    unsigned char      *p_img;
    int     scan_distance, scan_delta;
    int     n = 0;
    int     walked_distance = 0;
    

    if ( p_num_elements )
    {
	*p_num_elements = 0;
    }
    
    if (max_elements <= 0)
    {
	error = -2;
	return (-1);
    }

    if ( validate_horiz_scan_boundaries(row, col1, col2, scan_method) < 0 )
    {
	error = -3;
	return (-1);
    }

    p_img = ImageInfo.pImage + (row * ImageInfo.Pitch) + col1;

    if ( col1 <= col2 )
    {
	scan_distance = col2 - col1;
	scan_delta = scan_increment;
    }
    else
    {
	scan_distance = col1 - col2;
	scan_delta = -scan_increment;
    }

    while (walked_distance <= scan_distance && n < max_elements)
    {
	*p_scan_data++ = (unsigned int) *p_img;
//		*p_img = 10;	// draw the mtf roi area
	++n;
	walked_distance += scan_increment;
	p_img += scan_delta;
    }

    if ( p_num_elements )
    {
	*p_num_elements = n;
    }

    return n;
}

static void ScanDerivatives(unsigned int *p_scan_data,  /* Input:  scanned data */
			    int num_elements,           /* Input:  number of scanned elements */
			    int *p_deriv_data)          /* Output: deriv. data, the size of the array should be the same num_elements as p_scan_data */
{
    int     i;


    p_deriv_data[0] = p_deriv_data[num_elements - 1] = 0;

    for (i=1; i < num_elements - 1; ++i)
    {
	p_deriv_data[i] = (int)p_scan_data[i + 1] - (int)p_scan_data[i - 1];
    }
}

/* Return: 1 if the edge found; 0 if not */
static int find_next_edge(int *p_deriv_data,       /* Input:  deriv. data */
			  int num_elements,        /* Input:  number of elements in p_deriv_data */
			  int start_element,       /* Input:  index of the first element in p_deriv_data to begin from */
			  unsigned int deriv_threshold, /* Input:  deriv. threshold criteria */
			  int *p_edge_index,       /* Output: edge index to p_deriv_data */
			  int *p_edge_color,       /* Output: the color of the edge: +1 for black-to-white; -1 for white-to-back */
			  int *p_start_of_edge,    /* Output: start-of-edge index to p_deriv_data */
			  int *p_end_of_edge)      /* Output: end-of-edge index to p_deriv_data */
{
    int     n;
    int     der1;
    int     der1_color;
    int     der2;
    
    
    n = start_element;
    if ( n == 0 )
    {
	n = 1;
    }
    if ( n >= num_elements - 1 )
    {
	error = -4;
	return 0;
    }
    
    der2 = p_deriv_data[n];

    while ( n < num_elements - 1 )
    {
	der1 = der2;
	der1_color = der1 > 0 ? 1 : -1;
	der2 = p_deriv_data[n + 1];
        
	if ( abs(der1) > (int) deriv_threshold )
	{
	    *p_edge_color = der1_color;
	    *p_start_of_edge = n - 1;
            
	    /* Find the edge point as the point of maximum derivative */
            
	    while ( der2 * der1_color > abs(der1) )
	    {
		++n;
		if ( n == num_elements - 1 )
		{
		    der2 = 0;
		    break;
		}
                
		der1 = der2;
		der2 = p_deriv_data[n + 1];
	    }
            
	    *p_edge_index = n;
            
	    /* Find the end of the edge */
            
	    while ( der2 * der1_color > 0 && n < num_elements - 1 )
	    {
		++n;
		der2 = p_deriv_data[n + 1];
	    }
            
	    *p_end_of_edge = n;
	    return 1;
	}
        
	++n;
    }

    /* No edge found, but check derivative of the last element */
    
    der1 = der2;
    der1_color = der1 > 0 ? 1 : -1;
    if ( abs(der1) > (int) deriv_threshold )
    {
	/* der1 meets criterion */
	*p_edge_color = der1_color;
	*p_start_of_edge = n - 1;
	*p_edge_index = n;
	*p_end_of_edge = n;
	return 1;
    }
    
    error = -5;
    return 0;
}

/* Return: number of detected edges */
static int ScanEdges(int *p_deriv_data,            /* Input:  deriv. data */
		     int num_elements,             /* Input:  number of elements in p_deriv_data */
		     unsigned int min_deriv_threshold, /* Input:  minimum deriv. threshold criteria */
		     int *p_edge_data,             /* Output: indices of the detected edges to p_deriv_data */
		     int max_edges,                /* Input:  size of the p_edge_data array */
		     int *p_num_edges)             /* Output: number of edges returned in p_edge_data */
{
    int             edge_index;
    int             edge_color;
    int             start_of_edge;
    int             end_of_edge;
    int             start_element = 0;
    int             num_edges = 0;
    int             prev_edge_color = 0;
    unsigned int    deriv_threshold = min_deriv_threshold;


    while ( num_edges < max_edges &&
            find_next_edge(p_deriv_data,
                           num_elements,
                           start_element,
                           deriv_threshold,
                           &edge_index,
                           &edge_color,
                           &start_of_edge,
                           &end_of_edge) )
    {
	if ( edge_color != prev_edge_color )
	{
	    p_edge_data[num_edges++] = edge_index;
	}

	start_element = end_of_edge + 1;
	deriv_threshold = abs(p_deriv_data[edge_index]) / 3;
	if ( deriv_threshold < min_deriv_threshold )
	{
	    deriv_threshold = min_deriv_threshold;
	}

	prev_edge_color = edge_color;
    }

    *p_num_edges = num_edges;
    return num_edges;
}

static int     g_edge_data[MAX_COLS];
/* Return: 1 if reference zone found; 0 if not */
static int FindQARefZone1(int *p_deriv_data,       /* Input:  deriv. data */
			  int num_elements,        /* Input:  number of elements in p_deriv_data */
			  int *p_1,                /* Output: 1st point index to p_deriv_data */
			  int *p_2,                /* Output: 2nd point index to p_deriv_data */
			  int *p_3,                /* Output: last point index to p_deriv_data */
			  int *p_color)            /* Output: +1 for white-half followed by black-half; -1 for black-half followed by white-half */
{
    int     edge, edge_width1, edge_width2;
    int     num_edges;
    int     *edge_data = g_edge_data;


    ScanEdges(p_deriv_data,
              num_elements,
              MIN_DERIV_THRESHOLD,
              edge_data,
              MAX_COLS,
              &num_edges);

    if ( num_edges < 3 )
    {
	error = -6;
	return 0;
    }

    for (edge=1; edge < num_edges - 1; ++edge)
    {
	edge_width1 = abs(edge_data[edge] - edge_data[edge - 1]);
	edge_width2 = abs(edge_data[edge + 1] - edge_data[edge]);

	if ( 
	    edge_width1 >= MIN_QA_REF_ZONE_WIDTH &&
	    edge_width2 >= MIN_QA_REF_ZONE_WIDTH &&
	    edge_width1 <= MAX_QA_REF_ZONE_WIDTH &&
	    edge_width2 <= MAX_QA_REF_ZONE_WIDTH &&
	    edge_width1 <= 2 * edge_width2 &&
	    edge_width2 <= 2 * edge_width1
	    )
	{
	    int     p1_ix = edge_data[edge - 1];
	    int     p2_ix = edge_data[edge];
	    int     p3_ix = edge_data[edge + 1];
	    int     color = p_deriv_data[p1_ix] < 0 ? -1 : +1;
	    int     num_edges_after_white, last_required_ruling_edge_ix, min_required_ruling_distance;


	    /* check the area following the white-half -- it should contain some frequent small bars */

	    if ( color < 0 )
	    {
		/* black-half is followed by the white-half */
		num_edges_after_white = (num_edges - 1) - (edge + 1);
	    }
	    else
	    {
		/* white-half is followed by the black-half */
		num_edges_after_white = edge - 1;
	    }

	    if ( num_edges_after_white >= MIN_NUM_RULING_EDGES )
	    {
		if ( color < 0 )
		{
		    /* black-half is followed by the white-half */
		    last_required_ruling_edge_ix = edge_data[edge + 1 + MIN_NUM_RULING_EDGES];
		    min_required_ruling_distance = last_required_ruling_edge_ix - p3_ix;
		}
		else
		{
		    /* white-half is followed by the black-half */
		    last_required_ruling_edge_ix = edge_data[edge - 1 - MIN_NUM_RULING_EDGES];
		    min_required_ruling_distance = p1_ix - last_required_ruling_edge_ix;
		}

		if ( min_required_ruling_distance <= MIN_RULING_DISTANCE )
		{
		    /* got it! */
		    *p_1 = p1_ix;
		    *p_2 = p2_ix;
		    *p_3 = p3_ix;
		    *p_color = color;
		    return 1;
		}
	    }
	}
    }

    error = -7;
    return 0;
}

static unsigned int get_pixel_avg(unsigned int *scan_data, int i, int j)
{
    int             element_num, last_element, num_elements;
    unsigned int    pixel_sum;


    if ( i <= j )
    {
	element_num = i;
	last_element = j;
    }
    else
    {
	element_num = j;
	last_element = i;
    }

    num_elements = last_element - element_num + 1;

    for (pixel_sum=0; element_num <= last_element; ++element_num)
    {
	pixel_sum += scan_data[element_num];
    }

    return (pixel_sum / num_elements);
}

static int compare_pixels_descending(const void *data1, const void *data2)
{
    unsigned int    pixel1 = *((unsigned int *) data1);
    unsigned int    pixel2 = *((unsigned int *) data2);
    
    if (pixel1 < pixel2)
    {
	return 1;
    }
    else if (pixel1 > pixel2)
    {
	return (-1);
    }
    else
    {
	return 0;
    }
}

/************************************************************************/

static unsigned int    g_peaks[MAX_COLS];
static unsigned int get_pixel_max(unsigned int *scan_data, int i, int j)
{
    int             element_num, last_element/*, num_elements*/;
    unsigned int    *peaks = g_peaks;
    int             num_peaks = 0;
    int            going_up;


    if ( i <= j )
    {
	element_num = i;
	last_element = j;
    }
    else
    {
	element_num = j;
	last_element = i;
    }

    /*num_elements = last_element - element_num + 1;*/

    /* Collect the peaks */

    for (element_num+=1, going_up=TRUE; element_num <= last_element; ++element_num)
    {
	if ( going_up && 
	     scan_data[element_num] < scan_data[element_num - 1] )
	{
	    if ( num_peaks < MAX_COLS )
	    {
		peaks[num_peaks++] = scan_data[element_num - 1];
	    }
	    going_up = FALSE;
	    continue;
	}

	if ( going_up && 
	     scan_data[element_num] == scan_data[element_num - 1] &&
	     element_num == last_element )
	{
	    if ( num_peaks < MAX_COLS )
	    {
		peaks[num_peaks++] = scan_data[element_num];
	    }
	    continue;
	}

	if ( !going_up && 
	     scan_data[element_num] > scan_data[element_num - 1] )
	{
	    going_up = TRUE;
	    if ( element_num == last_element )
	    {
		if ( num_peaks < MAX_COLS )
		{
		    peaks[num_peaks++] = scan_data[element_num];
		}
	    }
	    continue;
	}
    }

    qsort((void *)peaks, num_peaks, sizeof(unsigned int), compare_pixels_descending);
    return peaks[num_peaks/4];
}

static int compare_pixels_ascending(const void *data1, const void *data2)
{
    unsigned int    pixel1 = *((unsigned int *) data1);
    unsigned int    pixel2 = *((unsigned int *) data2);
    
    if (pixel1 < pixel2)
    {
	return (-1);
    }
    else if (pixel1 > pixel2)
    {
	return 1;
    }
    else
    {
	return 0;
    }
}

/************************************************************************/

static unsigned int    g_valleys[MAX_COLS];
static unsigned int get_pixel_min(unsigned int *scan_data, int i, int j)
{
    int             element_num, last_element/*, num_elements*/;
    unsigned int    *valleys = g_valleys;
    int             num_valleys = 0;
    int            going_up;


    if ( i <= j )
    {
	element_num = i;
	last_element = j;
    }
    else
    {
	element_num = j;
	last_element = i;
    }

    /*num_elements = last_element - element_num + 1;*/

    /* Collect the valleys */

    for (element_num+=1, going_up=FALSE; element_num <= last_element; ++element_num)
    {
	if ( !going_up && 
	     scan_data[element_num] > scan_data[element_num - 1] )
	{
	    if ( num_valleys < MAX_COLS )
	    {
		valleys[num_valleys++] = scan_data[element_num - 1];
	    }
	    going_up = TRUE;
	    continue;
	}

	if ( !going_up && 
	     scan_data[element_num] == scan_data[element_num - 1] &&
	     element_num == last_element )
	{
	    if ( num_valleys < MAX_COLS )
	    {
		valleys[num_valleys++] = scan_data[element_num];
	    }
	    continue;
	}

	if ( going_up && 
	     scan_data[element_num] < scan_data[element_num - 1] )
	{
	    going_up = FALSE;
	    if ( element_num == last_element )
	    {
		if ( num_valleys < MAX_COLS )
		{
		    valleys[num_valleys++] = scan_data[element_num];
		}
	    }
	    continue;
	}
    }

    qsort((void *)valleys, num_valleys, sizeof(unsigned int), compare_pixels_ascending);
    return valleys[num_valleys/4];
}


static unsigned int    h_scan_data[MAX_COLS];
static int             h_deriv_data[MAX_COLS];
static float           h_vals[MAX_COLS];
static int do_mtf_horiz(int x1, int y1, int x2, int y2, float *ret)
{
    int             step;
    int             row;
    int             num_elements, last_element;
    int             num_rows, row_cnt;
    int             /*p_1, p_2, */p_3/*, color*/;
    unsigned int    *scan_data = h_scan_data;
    int             *deriv_data = h_deriv_data;
    float           *vals = h_vals;
    float           val;
    int             num_vals, i;
    unsigned int    /*white_ref, black_ref, */ruling_max, ruling_min;


    int find_bar = 0;
    
    if ( y1 <= y2 )
    {
	num_rows = y2 - y1 + 1;
	step = 1;
    }
    else
    {
	num_rows = y1 - y2 + 1;
	step = -1;
    }

    for (row_cnt=0, num_vals=0, row = y1; row_cnt < num_rows; ++row_cnt, row += step)
    {
	if ( ScanHorizLine(row,
			   x1,
			   x2,
			   0,
			   1,
			   scan_data,
			   MAX_COLS,
			   &num_elements) < 0 )
	{
	    error = -8;
	    return (-1);
	}

	ScanDerivatives(scan_data, num_elements, deriv_data);

	int     num_edges;
	int     *edge_data = g_edge_data;

	ScanEdges(deriv_data,
		  num_elements,
		  MIN_DERIV_THRESHOLD,
		  edge_data,
		  MAX_COLS,
		  &num_edges);
	if (num_edges < MIN_NUM_RULING_EDGES)
	    continue;

	int edge = 1;
	p_3 = edge_data[edge];

	int last_required_ruling_edge_ix, min_required_ruling_distance;
	last_required_ruling_edge_ix = edge_data[edge + MIN_NUM_RULING_EDGES];
	min_required_ruling_distance = last_required_ruling_edge_ix - p_3;
	if ( min_required_ruling_distance > MIN_RULING_DISTANCE )
	{
	    continue;
	}

	find_bar = 1;

	if ( (num_elements - p_3) < (2 * REF_ZONE_OFFSET) )
        {
	    continue;
        }
            
	if ( (num_elements - p_3) > 100 )
        {
	    last_element = p_3 + 100;
        }
	else
        {
	    last_element = num_elements - 1;
        }
            
	ruling_max = get_pixel_max(scan_data, p_3 + REF_ZONE_OFFSET, last_element);
	ruling_min = get_pixel_min(scan_data, p_3 + REF_ZONE_OFFSET, last_element);

	//PRINTF("p3:%d max:%d min:%d\n", p_3, ruling_max, ruling_min);

	val = (float)(ruling_max - ruling_min) / (float)(ruling_max + ruling_min);

	if ( val > 1.0 )
	{
	    /* something is wrong -- probably wrong ref. zone */
	    continue;
	}

	vals[num_vals++] = val;
    }

    if (!find_bar) {
	//PRINTF("no bar be found\n");
	error = -9;
	return -1;
    }

    if ( row_cnt > NUM_LINES_DEBUG && num_vals < MIN_LINES_REQUIRED )
    {
	//PRINTF("calc mtf lines is too little\n");
	error = -10;
	return (-1);
    }

    if ( num_vals == 0 )
    {   /* operation failed */
	//PRINTF("can't get a mtf value\n");
	error = -11;
	return (-1);
    }
    else
    {
	for (i=0, val=0; i < num_vals; ++i)
	{
	    val += vals[i];
	}

	val /= num_vals;
    }


    *ret = val;
    return 0;
}

static int
validate_vert_scan_boundaries(int col, int row1, int row2, int scan_method)
{
    int     size_x = ImageInfo.SizeX;
    int     size_y = ImageInfo.SizeY;


    if (col < 0 ||
	col >= size_x ||
	row1 < 0 ||
	row1 >= size_y ||
	row2 < 0 ||
	row2 >= size_y)
    {
	error = -12;
	return (-1);
    }

    return 0;
}

static int                                     /* Return: number of scanned elements if successful, or (-1) if error */
ScanVertLine(int col,                   /* Input:  scanning column */
             int row1,                  /* Input:  scan start row */
             int row2,                  /* Input:  scan end row */
             int scan_method,           /* Input:  scan method (currently not used) */
             int scan_increment,        /* Input:  scan resolution in pixels (can be >= 1) */
             unsigned int *p_scan_data, /* Output: scanned data */
             int max_elements,          /* Input:  size of the scan_data buffer */
             int *p_num_elements)       /* Output: number of elements returned in scan_data buffer */
{
    unsigned char      *p_img;
    int     scan_distance, scan_delta;
    int     n = 0;
    int     walked_distance = 0;
    

    if ( p_num_elements )
    {
	*p_num_elements = 0;
    }
    
    if (max_elements <= 0)
    {
	error = -13;
	return (-1);
    }

    if ( validate_vert_scan_boundaries(col, row1, row2, scan_method) < 0 )
    {
	error = -14;
	return (-1);
    }

    p_img = ImageInfo.pImage + (row1 * ImageInfo.Pitch) + col;

    if ( row1 <= row2 )
    {
	scan_distance = row2 - row1;
	scan_delta = scan_increment * ImageInfo.Pitch;
    }
    else
    {
	scan_distance = row1 - row2;
	scan_delta = -(scan_increment * ImageInfo.Pitch);
    }

    while (walked_distance <= scan_distance && n < max_elements)
    {
	*p_scan_data++ = (unsigned int) *p_img;
	++n;
	walked_distance += scan_increment;
	p_img += scan_delta;
    }

    if ( p_num_elements )
    {
	*p_num_elements = n;
    }

    return n;
}

static unsigned int    v_scan_data[MAX_ROWS];
static int             v_deriv_data[MAX_ROWS];
static float           v_vals[MAX_ROWS];
static int do_mtf_vert(int x1, int y1, int x2, int y2, float *ret)
{
    int             step;
    int             col;
    int             num_elements, last_element;
    int             num_cols, col_cnt;
    int             p_1, p_2, p_3, color;
    unsigned int    *scan_data = v_scan_data;
    int             *deriv_data = v_deriv_data;
    float           *vals = v_vals;
    float           val;
    int             num_vals, i;
    unsigned int    white_ref, black_ref, ruling_max, ruling_min;

    
    if ( x1 <= x2 )
    {
	num_cols = x2 - x1 + 1;
	step = 1;
    }
    else
    {
	num_cols = x1 - x2 + 1;
	step = -1;
    }

    for (col_cnt=0, num_vals=0, col = x1; col_cnt < num_cols; ++col_cnt, col += step)
    {
	if ( ScanVertLine(col,
                          y1,
                          y2,
                          0,
                          1,
                          scan_data,
                          MAX_ROWS,
                          &num_elements) < 0 )
	{
	    error = -15;
	    return (-1);
	}

	ScanDerivatives(scan_data, num_elements, deriv_data);

	if ( !FindQARefZone1(deriv_data,
                             num_elements,
                             &p_1,
                             &p_2,
                             &p_3,
                             &color) )
	{
	    /* Ref. zone not found */
	    continue;
	}

	/* sanity check */
	if ( abs(p_1 - p_2) < (2 * REF_ZONE_OFFSET) || abs(p_2 - p_3) < (2 * REF_ZONE_OFFSET) )
	{
	    continue;
	}

	if ( color < 0 )
	{
	    /* black-half followed by the white-half */

	    if ( (num_elements - p_3) < (2 * REF_ZONE_OFFSET) )
	    {
		continue;
	    }
            
	    if ( (num_elements - p_3) > 100 )
	    {
		last_element = p_3 + 100;
	    }
	    else
	    {
		last_element = num_elements - 1;
	    }
            
	    white_ref = get_pixel_avg(scan_data, p_2 + REF_ZONE_OFFSET, p_3 - REF_ZONE_OFFSET);
	    black_ref = get_pixel_avg(scan_data, p_1 + REF_ZONE_OFFSET, p_2 - REF_ZONE_OFFSET);
	    ruling_max = get_pixel_max(scan_data, p_3 + REF_ZONE_OFFSET, last_element);
	    ruling_min = get_pixel_min(scan_data, p_3 + REF_ZONE_OFFSET, last_element);
	}
	else
	{
	    /* white-half followed by the black-half */

	    if ( p_1 < (2 * REF_ZONE_OFFSET) )
	    {
		continue;
	    }

	    if ( p_1 > 100 )
	    {
		last_element = p_1 - 100;
	    }
	    else
	    {
		last_element = 0;
	    }

	    white_ref = get_pixel_avg(scan_data, p_1 + REF_ZONE_OFFSET, p_2 - REF_ZONE_OFFSET);
	    black_ref = get_pixel_avg(scan_data, p_2 + REF_ZONE_OFFSET, p_3 - REF_ZONE_OFFSET);
	    ruling_max = get_pixel_max(scan_data, p_1 - REF_ZONE_OFFSET, last_element);
	    ruling_min = get_pixel_min(scan_data, p_1 - REF_ZONE_OFFSET, last_element);
	}

	val = ( (float)(ruling_max - ruling_min) / (float)(ruling_max + ruling_min) ) / 
	    ( (float)(white_ref - black_ref) / (float)(white_ref + black_ref) );

	if ( val > 1.0 )
	{
	    /* something is wrong -- probably wrong ref. zone */
	    continue;
	}

	vals[num_vals++] = val;
    }

    if ( col_cnt > NUM_LINES_DEBUG && num_vals < MIN_LINES_REQUIRED )
    {   /* operation failed */
	error = -16;
	return (-1);
    }

    if ( num_vals == 0 )
    {   /* operation failed */
	error = -17;
	return (-1);
    }
    else
    {
	for (i=0, val=0; i < num_vals; ++i)
	{
	    val += vals[i];
	}

	val /= num_vals;
    }

    *ret = val;
    return 0;
}


/*
 * image_addr:   image buffer address
 * image_width:  the pixel number of image width
 * image_height: the pixel number of image height
 * x1:           coordinate start of X
 * x2:           coordinate end of X
 * y1:           coordinate start of Y
 * y2:           coordinate end of Y
 * hor:          calculating mtf along horizon or column
 * mtf_value:    return the mtf value
 * return@       if success, return 0; otherwise < 0
 */
int lib_mtf(unsigned char *image_addr,
            int image_width,
            int image_height,
            int x1,
            int x2,
            int y1,
            int y2,
            int horizon,
            float *mtf_value)
{
    ImageInfo.pImage = image_addr;
    ImageInfo.SizeX  = image_width;
    ImageInfo.SizeY  = image_height;
    ImageInfo.Pitch  = image_width;
    *mtf_value = 0;

    if (horizon) {
	if (do_mtf_horiz(x1, y1, x2, y2, mtf_value) < 0)
	{
	    return error;
	}
    } else {
	if (do_mtf_vert(x1, y1, x2, y2, mtf_value) < 0)
	{
	    return error;
	}
    }

    return 0;
}


