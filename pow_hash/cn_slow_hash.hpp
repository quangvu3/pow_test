#pragma once

#include <inttypes.h>
#include <stddef.h>

#ifdef __GNUC__
#include <mm_malloc.h>
#else
#include <malloc.h>
#endif // __GNUC__

// This cruft avoids casting-galore and allows us not to worry about sizeof(void*)
union cn_sptr
{
	cn_sptr() : as_void(nullptr) {}
	cn_sptr(uint64_t* ptr) { as_uqword = ptr; }
	cn_sptr(uint32_t* ptr) { as_udword = ptr; }
	cn_sptr(uint8_t* ptr) { as_byte = ptr; }

	void* as_void;
	uint8_t* as_byte;
	uint64_t* as_uqword;
	int32_t* as_dword;
	uint32_t* as_udword;
};

template<size_t MEMORY, size_t ITER>
class cn_slow_hash
{
public:
	cn_slow_hash()
	{
		lpad.as_void = _mm_malloc(MEMORY, 4096);
		spad.as_void = _mm_malloc(200, 4096);
	}

	~cn_slow_hash()
	{
		_mm_free(lpad.as_void);
		_mm_free(spad.as_void);
	}

	void hash(const void* in, size_t len, void* out);

private:
	static constexpr size_t MASK = ((MEMORY-1) >> 4) << 4;

	inline uint8_t* scratchpad_ptr(uint32_t idx) { return lpad.as_byte + (idx & MASK); }

	void explode_scratchpad();
	void implode_scratchpad();

	cn_sptr lpad;
	cn_sptr spad;
};
