#ifndef SAVEGAME_ENUMMAP_HELPER_H
#define SAVEGAME_ENUMMAP_HELPER_H

//#include "CvSavegame.h"



	// mode byte count
	// mode 0: 1 first 1 last
	// mode 1: 2 first 1 last
	// mode 2: 2 first 2 last


	template<typename T>
	class SavegameEnumMapToken
	{
	public:
		T getFirstIndex() const;
		T getLastIndex() const;

		bool isEmpty() const;
		bool isLast() const;

		const int m_iMode;

	protected:
		SavegameEnumMapToken(int iLength);

		union
		{
			unsigned int m_iInt;
			unsigned short m_iEmpty;
			byte m_Array[4];
			// index 0: always present
			// index 1: only saved if mode isn't 0
			// index 2: only saved if m_bMultiByte and not empty
			// index 3: only saved if mode is 2
			struct
			{
				unsigned int m_bMultiByte  :  1;
				unsigned int m_bLastToken  :  1;
				unsigned int m_iFirstIndex : 14;
				unsigned int m_iLastIndex  : 16;
			};
		};
	};

	template<typename T, typename T2>
	class SavegameEnumMapTokenRead : public SavegameEnumMapToken<T>
	{
	public:
		SavegameEnumMapTokenRead(CvSavegameReader& kReader);
	};

	template<typename T, typename T2>
	class SavegameEnumMapTokenWrite : public SavegameEnumMapToken<T>
	{
	public:
		SavegameEnumMapTokenWrite();

		void setStart(T eFirstIndex);
		void setEnd(T eLastIndex);
		void setLast();

		void write(CvSavegameWriter& m_kWriter);
		void writeEmpty(CvSavegameWriter& m_kWriter);
	};


	template<typename T>
	inline SavegameEnumMapToken<T>::SavegameEnumMapToken(int iLength)
		: m_iInt(0)
		, m_iMode(iLength <= 64 ? 0 : iLength <= 256 ? 1 : 2)
	{
	}


	template<typename T>
	inline T SavegameEnumMapToken<T>::getFirstIndex() const
	{
		return (T)m_iFirstIndex;
	}
	

	template<typename T>
	inline T SavegameEnumMapToken<T>::getLastIndex() const
	{
		return (T)m_iLastIndex;
	}

	template<typename T>
	inline bool SavegameEnumMapToken<T>::isEmpty() const
	{
		return m_iMode == 0 ? m_Array[0] == 0xFF : m_iEmpty == 0xFFFF;
	}

	template<typename T>
	inline bool SavegameEnumMapToken<T>::isLast() const
	{
		return m_bLastToken;
	}



	template<typename T, typename T2>
	inline SavegameEnumMapTokenRead<T, T2>::SavegameEnumMapTokenRead(CvSavegameReader& kReader)
		: SavegameEnumMapToken<T>(kReader.GetXmlSize(VARINFO<T2>::JIT) > 0 ? kReader.GetXmlSize(VARINFO<T2>::JIT) : VARINFO<T2>::NUM_ELEMENTS)
	{
		kReader.Read(m_Array[0]);
		if (m_iMode != 0) // two byte start index
		{
			kReader.Read(m_Array[1]);
		}
		if (isEmpty())
		{
			return;
		}

		m_iLastIndex = m_iFirstIndex;

		if (m_bMultiByte)
		{
			kReader.Read(m_Array[2]);
			if (m_iMode == 2) // two byte last index 
			{
				kReader.Read(m_Array[3]);
			}
		}
	}

	
	template<typename T, typename T2>
	inline SavegameEnumMapTokenWrite<T, T2>::SavegameEnumMapTokenWrite()
		: SavegameEnumMapToken<T>(VARINFO<T2>::NUM_ELEMENTS)
	{
	}

	template<typename T, typename T2>
	inline void SavegameEnumMapTokenWrite<T, T2>::setStart(T eFirstIndex)
	{
		//m_bIsSet = true;
		m_iFirstIndex = eFirstIndex;
	}

	template<typename T, typename T2>
	inline void SavegameEnumMapTokenWrite<T, T2>::setEnd(T eLastIndex)
	{
		m_iLastIndex = eLastIndex;
	}

	template<typename T, typename T2>
	inline void SavegameEnumMapTokenWrite<T, T2>::setLast()
	{
		m_bLastToken = 1;
	}

	template<typename T, typename T2>
	inline void SavegameEnumMapTokenWrite<T, T2>::write(CvSavegameWriter& kWriter)
	{
		m_bMultiByte = m_iFirstIndex == m_iLastIndex ? 0 : 1;
		kWriter.Write(m_Array[0]);
		if (m_iMode != 0)
		{
			kWriter.Write(m_Array[1]);
		}
		if (m_bMultiByte == 0)
		{
			return;
		}
		kWriter.Write(m_Array[2]);
		if (m_iMode == 2)
		{
			kWriter.Write(m_Array[3]);
		}
	}

	template<typename T, typename T2>
	inline void SavegameEnumMapTokenWrite<T, T2>::writeEmpty(CvSavegameWriter& kWriter)
	{
		m_iEmpty = 0xFFFF;
		if (m_iMode == 0)
		{
			kWriter.Write(m_Array[0]);
		}
		else
		{
			kWriter.Write(m_iEmpty);
		}
	}

#endif
