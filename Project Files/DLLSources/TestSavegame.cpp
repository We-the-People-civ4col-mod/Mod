
#include "CvGameCoreDLL.h"
#include "WTPTestFramework.h"

#ifdef FASSERT_ENABLE

#include "CvTradeRoute.h"

// ============================================================================
// MemoryStream — minimal FDataStreamBase for testing
// ============================================================================

class MemoryStream : public FDataStreamBase
{
public:
	MemoryStream() : m_iPos(0) {}

	// -- Position/State --
	void Rewind()                              { m_iPos = 0; }
	bool AtEnd()                               { return m_iPos >= m_data.size(); }
	void FastFwd()                             { m_iPos = m_data.size(); }
	unsigned int GetPosition() const           { return m_iPos; }
	void SetPosition(unsigned int position)    { m_iPos = position; }
	unsigned int GetEOF() const                { return m_data.size(); }
	unsigned int GetSizeLeft() const           { return m_data.size() - m_iPos; }
	void Truncate()                            { m_data.resize(m_iPos); }
	void Flush()                               {}
	void CopyToMem(void* mem)                  { memcpy(mem, &m_data[0], m_data.size()); }

	// -- Raw read/write helpers --
	void rawRead(void* dest, unsigned int bytes)
	{
		FAssert(m_iPos + bytes <= m_data.size());
		memcpy(dest, &m_data[m_iPos], bytes);
		m_iPos += bytes;
	}

	void rawWrite(const void* src, unsigned int bytes)
	{
		if (m_iPos + bytes > m_data.size())
			m_data.resize(m_iPos + bytes);
		memcpy(&m_data[m_iPos], src, bytes);
		m_iPos += bytes;
	}

	// -- String Write --
	unsigned int WriteString(const wchar* szName)
	{
		unsigned int len = szName ? (unsigned int)wcslen(szName) : 0;
		Write(len);
		if (len > 0) Write((int)len, (const short*)szName);
		return len;
	}
	unsigned int WriteString(const char* szName)
	{
		unsigned int len = szName ? (unsigned int)strlen(szName) : 0;
		Write(len);
		if (len > 0) Write((int)len, szName);
		return len;
	}
	unsigned int WriteString(const std::string& s)  { return WriteString(s.c_str()); }
	unsigned int WriteString(const std::wstring& s) { return WriteString(s.c_str()); }
	unsigned int WriteString(int count, std::string values[])
	{
		for (int i = 0; i < count; ++i) WriteString(values[i]);
		return 0;
	}
	unsigned int WriteString(int count, std::wstring values[])
	{
		for (int i = 0; i < count; ++i) WriteString(values[i]);
		return 0;
	}

	// -- String Read --
	unsigned int ReadString(char* szName)
	{
		unsigned int len; Read(&len);
		if (len > 0) Read((int)len, szName);
		szName[len] = 0;
		return len;
	}
	unsigned int ReadString(wchar* szName)
	{
		unsigned int len; Read(&len);
		if (len > 0) Read((int)len, (short*)szName);
		szName[len] = 0;
		return len;
	}
	unsigned int ReadString(std::string& s)
	{
		unsigned int len; Read(&len);
		s.resize(len);
		if (len > 0) Read((int)len, &s[0]);
		return len;
	}
	unsigned int ReadString(std::wstring& s)
	{
		unsigned int len; Read(&len);
		s.resize(len);
		if (len > 0) Read((int)len, (short*)&s[0]);
		return len;
	}
	unsigned int ReadString(int count, std::string values[])
	{
		for (int i = 0; i < count; ++i) ReadString(values[i]);
		return 0;
	}
	unsigned int ReadString(int count, std::wstring values[])
	{
		for (int i = 0; i < count; ++i) ReadString(values[i]);
		return 0;
	}
	char* ReadString()
	{
		unsigned int len; Read(&len);
		char* buf = new char[len + 1];
		if (len > 0) Read((int)len, buf);
		buf[len] = 0;
		return buf;
	}
	wchar* ReadWideString()
	{
		unsigned int len; Read(&len);
		wchar* buf = new wchar[len + 1];
		if (len > 0) Read((int)len, (short*)buf);
		buf[len] = 0;
		return buf;
	}

	// -- Primitive Read (single) --
	void Read(char* v)           { rawRead(v, sizeof(char)); }
	void Read(byte* v)           { rawRead(v, sizeof(byte)); }
	void Read(bool* v)           { rawRead(v, sizeof(bool)); }
	void Read(short* v)          { rawRead(v, sizeof(short)); }
	void Read(unsigned short* v) { rawRead(v, sizeof(unsigned short)); }
	void Read(int* v)            { rawRead(v, sizeof(int)); }
	void Read(unsigned int* v)   { rawRead(v, sizeof(unsigned int)); }
	void Read(long* v)           { rawRead(v, sizeof(long)); }
	void Read(unsigned long* v)  { rawRead(v, sizeof(unsigned long)); }
	void Read(float* v)          { rawRead(v, sizeof(float)); }
	void Read(double* v)         { rawRead(v, sizeof(double)); }

	// -- Primitive Read (array) --
	void Read(int count, char values[])           { rawRead(values, count * sizeof(char)); }
	void Read(int count, byte values[])           { rawRead(values, count * sizeof(byte)); }
	void Read(int count, bool values[])           { rawRead(values, count * sizeof(bool)); }
	void Read(int count, short values[])          { rawRead(values, count * sizeof(short)); }
	void Read(int count, unsigned short values[]) { rawRead(values, count * sizeof(unsigned short)); }
	void Read(int count, int values[])            { rawRead(values, count * sizeof(int)); }
	void Read(int count, unsigned int values[])   { rawRead(values, count * sizeof(unsigned int)); }
	void Read(int count, long values[])           { rawRead(values, count * sizeof(long)); }
	void Read(int count, unsigned long values[])  { rawRead(values, count * sizeof(unsigned long)); }
	void Read(int count, float values[])          { rawRead(values, count * sizeof(float)); }
	void Read(int count, double values[])         { rawRead(values, count * sizeof(double)); }

	// -- Primitive Write (single) --
	void Write(char v)           { rawWrite(&v, sizeof(char)); }
	void Write(byte v)           { rawWrite(&v, sizeof(byte)); }
	void Write(bool v)           { rawWrite(&v, sizeof(bool)); }
	void Write(short v)          { rawWrite(&v, sizeof(short)); }
	void Write(unsigned short v) { rawWrite(&v, sizeof(unsigned short)); }
	void Write(int v)            { rawWrite(&v, sizeof(int)); }
	void Write(unsigned int v)   { rawWrite(&v, sizeof(unsigned int)); }
	void Write(long v)           { rawWrite(&v, sizeof(long)); }
	void Write(unsigned long v)  { rawWrite(&v, sizeof(unsigned long)); }
	void Write(float v)          { rawWrite(&v, sizeof(float)); }
	void Write(double v)         { rawWrite(&v, sizeof(double)); }

	// -- Primitive Write (array) --
	void Write(int count, const char values[])           { rawWrite(values, count * sizeof(char)); }
	void Write(int count, const byte values[])           { rawWrite(values, count * sizeof(byte)); }
	void Write(int count, const bool values[])           { rawWrite(values, count * sizeof(bool)); }
	void Write(int count, const short values[])          { rawWrite(values, count * sizeof(short)); }
	void Write(int count, const unsigned short values[]) { rawWrite(values, count * sizeof(unsigned short)); }
	void Write(int count, const int values[])            { rawWrite(values, count * sizeof(int)); }
	void Write(int count, const unsigned int values[])   { rawWrite(values, count * sizeof(unsigned int)); }
	void Write(int count, const long values[])           { rawWrite(values, count * sizeof(long)); }
	void Write(int count, const unsigned long values[])  { rawWrite(values, count * sizeof(unsigned long)); }
	void Write(int count, const float values[])          { rawWrite(values, count * sizeof(float)); }
	void Write(int count, const double values[])         { rawWrite(values, count * sizeof(double)); }

private:
	std::vector<byte> m_data;
	unsigned int m_iPos;
};

// ============================================================================
// Primitive Round-Trip Tests
// ============================================================================

WTP_TEST(Savegame, IntRoundTrip)
{
	MemoryStream stream;
	int iWrite = 42;
	stream.Write(iWrite);

	stream.Rewind();
	int iRead = 0;
	stream.Read(&iRead);
	WTP_ASSERT_EQ(42, iRead);
}

WTP_TEST(Savegame, BoolRoundTrip)
{
	MemoryStream stream;
	stream.Write(true);
	stream.Write(false);

	stream.Rewind();
	bool b1 = false, b2 = true;
	stream.Read(&b1);
	stream.Read(&b2);
	WTP_ASSERT(b1 == true);
	WTP_ASSERT(b2 == false);
}

WTP_TEST(Savegame, ShortRoundTrip)
{
	MemoryStream stream;
	short sWrite = -12345;
	stream.Write(sWrite);

	stream.Rewind();
	short sRead = 0;
	stream.Read(&sRead);
	WTP_ASSERT_EQ(-12345, static_cast<int>(sRead));
}

WTP_TEST(Savegame, UnsignedShortRoundTrip)
{
	MemoryStream stream;
	unsigned short usWrite = 65535;
	stream.Write(usWrite);

	stream.Rewind();
	unsigned short usRead = 0;
	stream.Read(&usRead);
	WTP_ASSERT_EQ(65535, static_cast<int>(usRead));
}

WTP_TEST(Savegame, FloatRoundTrip)
{
	MemoryStream stream;
	float fWrite = 3.14f;
	stream.Write(fWrite);

	stream.Rewind();
	float fRead = 0.0f;
	stream.Read(&fRead);
	// Float comparison: check within tolerance
	WTP_ASSERT_MSG(fRead > 3.13f && fRead < 3.15f, "float round-trip mismatch");
}

WTP_TEST(Savegame, DoubleRoundTrip)
{
	MemoryStream stream;
	double dWrite = 2.718281828;
	stream.Write(dWrite);

	stream.Rewind();
	double dRead = 0.0;
	stream.Read(&dRead);
	WTP_ASSERT_MSG(dRead > 2.718 && dRead < 2.719, "double round-trip mismatch");
}

WTP_TEST(Savegame, MultipleValuesSequential)
{
	MemoryStream stream;
	stream.Write(100);
	stream.Write(static_cast<short>(200));
	stream.Write(true);
	stream.Write(static_cast<byte>(255));

	stream.Rewind();
	int i; short s; bool b; byte by;
	stream.Read(&i);
	stream.Read(&s);
	stream.Read(&b);
	stream.Read(&by);
	WTP_ASSERT_EQ(100, i);
	WTP_ASSERT_EQ(200, static_cast<int>(s));
	WTP_ASSERT(b == true);
	WTP_ASSERT_EQ(255, static_cast<int>(by));
}

WTP_TEST(Savegame, IntArrayRoundTrip)
{
	MemoryStream stream;
	int arrWrite[3] = {10, 20, 30};
	stream.Write(3, arrWrite);

	stream.Rewind();
	int arrRead[3] = {0, 0, 0};
	stream.Read(3, arrRead);
	WTP_ASSERT_EQ(10, arrRead[0]);
	WTP_ASSERT_EQ(20, arrRead[1]);
	WTP_ASSERT_EQ(30, arrRead[2]);
}

// ============================================================================
// Stream State Tests
// ============================================================================

WTP_TEST(Savegame, StreamPosition)
{
	MemoryStream stream;
	WTP_ASSERT_EQ(0, static_cast<int>(stream.GetPosition()));
	WTP_ASSERT_EQ(0, static_cast<int>(stream.GetEOF()));

	stream.Write(42);
	WTP_ASSERT_EQ(4, static_cast<int>(stream.GetPosition()));
	WTP_ASSERT_EQ(4, static_cast<int>(stream.GetEOF()));

	stream.Rewind();
	WTP_ASSERT_EQ(0, static_cast<int>(stream.GetPosition()));
	WTP_ASSERT_EQ(4, static_cast<int>(stream.GetEOF()));
	WTP_ASSERT_EQ(4, static_cast<int>(stream.GetSizeLeft()));
}

WTP_TEST(Savegame, StreamAtEnd)
{
	MemoryStream stream;
	WTP_ASSERT(stream.AtEnd());

	stream.Write(1);
	WTP_ASSERT(stream.AtEnd()); // write position is at end

	stream.Rewind();
	WTP_ASSERT(!stream.AtEnd());

	int dummy;
	stream.Read(&dummy);
	WTP_ASSERT(stream.AtEnd());
}

WTP_TEST(Savegame, StreamSetPosition)
{
	MemoryStream stream;
	stream.Write(10);
	stream.Write(20);
	stream.Write(30);

	// Read second value by seeking
	stream.SetPosition(4); // skip first int
	int val;
	stream.Read(&val);
	WTP_ASSERT_EQ(20, val);

	// Seek back to first
	stream.SetPosition(0);
	stream.Read(&val);
	WTP_ASSERT_EQ(10, val);
}

// ============================================================================
// String Round-Trip Tests
// ============================================================================

WTP_TEST(Savegame, StringRoundTrip)
{
	MemoryStream stream;
	stream.WriteString("hello");

	stream.Rewind();
	char buf[32];
	stream.ReadString(buf);
	WTP_ASSERT_MSG(strcmp(buf, "hello") == 0, "string round-trip mismatch");
}

WTP_TEST(Savegame, StdStringRoundTrip)
{
	MemoryStream stream;
	std::string sWrite = "test_string";
	stream.WriteString(sWrite);

	stream.Rewind();
	std::string sRead;
	stream.ReadString(sRead);
	WTP_ASSERT_MSG(sRead == "test_string", "std::string round-trip mismatch");
}

WTP_TEST(Savegame, EmptyStringRoundTrip)
{
	MemoryStream stream;
	stream.WriteString("");

	stream.Rewind();
	std::string sRead = "notempty";
	stream.ReadString(sRead);
	WTP_ASSERT_MSG(sRead.empty(), "empty string round-trip should produce empty");
}

// ============================================================================
// IDInfo Round-Trip Tests
// ============================================================================

WTP_TEST(Savegame, IDInfoDefaultRoundTrip)
{
	MemoryStream stream;
	IDInfo original;
	original.write(&stream);

	stream.Rewind();
	IDInfo loaded;
	loaded.eOwner = FIRST_PLAYER; // set non-default to verify read works
	loaded.iID = 999;
	loaded.read(&stream);

	WTP_ASSERT_EQ(NO_PLAYER, loaded.eOwner);
	WTP_ASSERT_EQ(-1, loaded.iID);
}

WTP_TEST(Savegame, IDInfoWithDataRoundTrip)
{
	MemoryStream stream;
	IDInfo original(FIRST_PLAYER, 42);
	original.write(&stream);

	stream.Rewind();
	IDInfo loaded;
	loaded.read(&stream);

	WTP_ASSERT(loaded == original);
	WTP_ASSERT_EQ(FIRST_PLAYER, loaded.eOwner);
	WTP_ASSERT_EQ(42, loaded.iID);
}

WTP_TEST(Savegame, IDInfoTradeRouteSentinelRoundTrip)
{
	// Test that off-map sentinel IDs survive serialization
	MemoryStream stream;

	IDInfo europeInfo(FIRST_PLAYER, CvTradeRoute::EUROPE_CITY_ID);
	IDInfo anywhereInfo(FIRST_PLAYER, CvTradeRoute::ANYWHERE_CITY_ID);

	europeInfo.write(&stream);
	anywhereInfo.write(&stream);

	stream.Rewind();

	IDInfo loadedEurope, loadedAnywhere;
	loadedEurope.read(&stream);
	loadedAnywhere.read(&stream);

	WTP_ASSERT(loadedEurope == europeInfo);
	WTP_ASSERT(loadedAnywhere == anywhereInfo);

	// Verify sentinel values survived round-trip
	WTP_ASSERT_EQ(CvTradeRoute::EUROPE_CITY_ID, loadedEurope.iID);
	WTP_ASSERT_EQ(CvTradeRoute::ANYWHERE_CITY_ID, loadedAnywhere.iID);
}

// ============================================================================
// JustInTimeArray Round-Trip Tests
// ============================================================================

WTP_TEST(Savegame, JITArrayEmptyRoundTrip)
{
	MemoryStream stream;
	JustInTimeArray<int> arrWrite(JIT_ARRAY_ROUTE);
	arrWrite.Write(&stream);

	stream.Rewind();
	JustInTimeArray<int> arrRead(JIT_ARRAY_ROUTE);
	arrRead.Read(&stream);

	// Note: JustInTimeArray::Read() always calls allocate(), even for empty arrays
	// So after round-trip, the array is allocated but all values are default (0)
	WTP_ASSERT_EQ(0, arrRead.get(0));
}

WTP_TEST(Savegame, JITArrayWithDataRoundTrip)
{
	MemoryStream stream;
	JustInTimeArray<int> arrWrite(JIT_ARRAY_ROUTE);
	arrWrite.set(42, 0);
	if (arrWrite.length() > 1)
	{
		arrWrite.set(99, 1);
	}
	arrWrite.Write(&stream);

	stream.Rewind();
	JustInTimeArray<int> arrRead(JIT_ARRAY_ROUTE);
	arrRead.Read(&stream);

	WTP_ASSERT(arrRead.isAllocated());
	WTP_ASSERT_EQ(42, arrRead.get(0));
	if (arrRead.length() > 1)
	{
		WTP_ASSERT_EQ(99, arrRead.get(1));
	}
}

WTP_TEST(Savegame, JITArrayResetAndRoundTrip)
{
	MemoryStream stream;
	JustInTimeArray<int> arrWrite(JIT_ARRAY_ROUTE);
	arrWrite.set(10, 0);
	arrWrite.reset();
	// Write an empty (reset) array
	arrWrite.Write(&stream);

	stream.Rewind();
	JustInTimeArray<int> arrRead(JIT_ARRAY_ROUTE);
	arrRead.set(777, 0); // pre-populate to verify reset works
	arrRead.Read(&stream);

	WTP_ASSERT_EQ(0, arrRead.get(0));
}

// ============================================================================
// Enum Round-Trip via FDataStreamBase Template
// ============================================================================

WTP_TEST(Savegame, EnumRoundTrip)
{
	MemoryStream stream;
	YieldTypes eWrite = YIELD_FOOD;
	stream.Write(static_cast<int>(eWrite));

	stream.Rewind();
	YieldTypes eRead = NO_YIELD;
	int iRead = 0;
	stream.Read(&iRead);
	eRead = static_cast<YieldTypes>(iRead);
	WTP_ASSERT_EQ(YIELD_FOOD, eRead);
}

WTP_TEST(Savegame, EnumNegativeRoundTrip)
{
	MemoryStream stream;
	YieldTypes eWrite = NO_YIELD;
	stream.Write(static_cast<int>(eWrite));

	stream.Rewind();
	YieldTypes eRead = YIELD_FOOD;
	int iRead = 0;
	stream.Read(&iRead);
	eRead = static_cast<YieldTypes>(iRead);
	WTP_ASSERT_EQ(NO_YIELD, eRead);
}

// ============================================================================
// Edge Cases
// ============================================================================

WTP_TEST(Savegame, LargeIntValues)
{
	MemoryStream stream;
	int iMax = 2147483647; // MAX_INT
	int iMin = -2147483647 - 1; // MIN_INT
	stream.Write(iMax);
	stream.Write(iMin);

	stream.Rewind();
	int iReadMax, iReadMin;
	stream.Read(&iReadMax);
	stream.Read(&iReadMin);
	WTP_ASSERT_EQ(iMax, iReadMax);
	WTP_ASSERT_EQ(iMin, iReadMin);
}

WTP_TEST(Savegame, ZeroLengthArrayRoundTrip)
{
	MemoryStream stream;
	// Write zero-count array (should write nothing)
	int dummy[1] = {0};
	stream.Write(0, dummy);

	WTP_ASSERT_EQ(0, static_cast<int>(stream.GetEOF()));
}

WTP_TEST(Savegame, TruncateStream)
{
	MemoryStream stream;
	stream.Write(10);
	stream.Write(20);
	stream.Write(30);
	WTP_ASSERT_EQ(12, static_cast<int>(stream.GetEOF()));

	stream.SetPosition(8); // after second int
	stream.Truncate();
	WTP_ASSERT_EQ(8, static_cast<int>(stream.GetEOF()));

	stream.Rewind();
	int v1, v2;
	stream.Read(&v1);
	stream.Read(&v2);
	WTP_ASSERT_EQ(10, v1);
	WTP_ASSERT_EQ(20, v2);
	WTP_ASSERT(stream.AtEnd());
}

// ============================================================================
// Test Runner
// ============================================================================

void RunSavegameTests()
{
	// Primitive round-trips
	WTP_RUN_SUITE_BEGIN();
	WTP_RUN_TEST(Savegame, IntRoundTrip);
	WTP_RUN_TEST(Savegame, BoolRoundTrip);
	WTP_RUN_TEST(Savegame, ShortRoundTrip);
	WTP_RUN_TEST(Savegame, UnsignedShortRoundTrip);
	WTP_RUN_TEST(Savegame, FloatRoundTrip);
	WTP_RUN_TEST(Savegame, DoubleRoundTrip);
	WTP_RUN_TEST(Savegame, MultipleValuesSequential);
	WTP_RUN_TEST(Savegame, IntArrayRoundTrip);
	WTP_RUN_SUITE_END("Savegame_Primitives");

	// Stream state
	WTP_RUN_SUITE_BEGIN();
	WTP_RUN_TEST(Savegame, StreamPosition);
	WTP_RUN_TEST(Savegame, StreamAtEnd);
	WTP_RUN_TEST(Savegame, StreamSetPosition);
	WTP_RUN_SUITE_END("Savegame_StreamState");

	// String round-trips
	WTP_RUN_SUITE_BEGIN();
	WTP_RUN_TEST(Savegame, StringRoundTrip);
	WTP_RUN_TEST(Savegame, StdStringRoundTrip);
	WTP_RUN_TEST(Savegame, EmptyStringRoundTrip);
	WTP_RUN_SUITE_END("Savegame_Strings");

	// IDInfo round-trips
	WTP_RUN_SUITE_BEGIN();
	WTP_RUN_TEST(Savegame, IDInfoDefaultRoundTrip);
	WTP_RUN_TEST(Savegame, IDInfoWithDataRoundTrip);
	WTP_RUN_TEST(Savegame, IDInfoTradeRouteSentinelRoundTrip);
	WTP_RUN_SUITE_END("Savegame_IDInfo");

	// JustInTimeArray round-trips
	WTP_RUN_SUITE_BEGIN();
	WTP_RUN_TEST(Savegame, JITArrayEmptyRoundTrip);
	WTP_RUN_TEST(Savegame, JITArrayWithDataRoundTrip);
	WTP_RUN_TEST(Savegame, JITArrayResetAndRoundTrip);
	WTP_RUN_SUITE_END("Savegame_JustInTimeArray");

	// Enum round-trips
	WTP_RUN_SUITE_BEGIN();
	WTP_RUN_TEST(Savegame, EnumRoundTrip);
	WTP_RUN_TEST(Savegame, EnumNegativeRoundTrip);
	WTP_RUN_SUITE_END("Savegame_Enums");

	// Edge cases
	WTP_RUN_SUITE_BEGIN();
	WTP_RUN_TEST(Savegame, LargeIntValues);
	WTP_RUN_TEST(Savegame, ZeroLengthArrayRoundTrip);
	WTP_RUN_TEST(Savegame, TruncateStream);
	WTP_RUN_SUITE_END("Savegame_EdgeCases");
}

#endif
