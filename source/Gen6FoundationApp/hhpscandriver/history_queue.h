template <class T, size_t size>
class History_queue
{
public:
	History_queue():
		position(0)
		{
		}

	void push(const T & v)
		{
			if (position == 0)
				position = size;
			--position;
  			ary[position] = v;
		}

	void dup()
	{
		push(ary[position]);
	}
	
	T & operator[](int idx) 
		{ 
			if (idx < 0)
			{
				idx += size;
				SDASSERT(idx >= 0);
			}
			SDASSERT(idx < size);

			idx += position;
			if (idx >= size)
				idx -= size;
			return ary[idx];
		}

	const T & operator[](int idx) const
		{ 
			return const_cast<History_queue &>(*this)[idx];
		}
	

private:
	size_t position;
	T ary[size];
};
