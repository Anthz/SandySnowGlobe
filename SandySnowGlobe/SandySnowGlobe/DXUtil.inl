namespace Memory
{
	template <typename T> inline void SafeRelease(T *t)
	{
		if (t)
		{
			t->Release();
			t = nullptr;
		}
	}

	template <typename T> inline void SafeDelete(T *t)
	{
		delete t;
		t = nullptr;
	}

	template <typename T> inline void SafeDeleteArr(T *t)
	{
		delete[] t;
		t = nullptr;
	}
}