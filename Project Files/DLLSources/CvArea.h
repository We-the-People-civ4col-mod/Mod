#pragma once

// area.h

#ifndef CIV4_AREA_H
#define CIV4_AREA_H

//#include "CvStructs.h"

class CvCity;
class CvPlot;

class CvArea
{

public:

  CvArea();
  virtual ~CvArea();

	void init(int iID, bool bWater);
	void reset();
	int calculateTotalBestNatureYield() const;

	int countCoastalLand() const;
	int countNumUniqueBonusTypes() const;

	int getID() const;
	void setID(int iID);
	int getNumTiles() const;
	bool isLake() const;
	void changeNumTiles(int iChange);
	int getNumOwnedTiles() const;
	int getNumUnownedTiles() const;
	void changeNumOwnedTiles(int iChange);
	int getNumRiverEdges() const;
	void changeNumRiverEdges(int iChange);
	int getNumCities() const;

	int getNumUnits() const;


	int getNumStartingPlots() const;
	void changeNumStartingPlots(int iChange);
	bool isWater() const;
	bool hasEurope() const;

	int getUnitsPerPlayer(PlayerTypes eIndex) const;
	void changeUnitsPerPlayer(PlayerTypes eIndex, int iChange);
	int getCitiesPerPlayer(PlayerTypes eIndex) const;
	void changeCitiesPerPlayer(PlayerTypes eIndex, int iChange);
	int getPopulationPerPlayer(PlayerTypes eIndex) const;
	void changePopulationPerPlayer(PlayerTypes eIndex, int iChange);
	int getPower(PlayerTypes eIndex) const;
	void changePower(PlayerTypes eIndex, int iChange);
	int getBestFoundValue(PlayerTypes eIndex) const;
	void setBestFoundValue(PlayerTypes eIndex, int iNewValue);
	int getNumRevealedTiles(TeamTypes eIndex) const;
	int getNumUnrevealedTiles(TeamTypes eIndex) const;
	void changeNumRevealedTiles(TeamTypes eIndex, int iChange);
	AreaAITypes getAreaAIType(TeamTypes eIndex) const;
	void setAreaAIType(TeamTypes eIndex, AreaAITypes eNewValue);
	CvCity* getTargetCity(PlayerTypes eIndex) const;
	void setTargetCity(PlayerTypes eIndex, CvCity* pNewValue);
	int getYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2) const;
	void changeYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2, int iChange);
	int getNumTrainAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2) const;
	void changeNumTrainAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2, int iChange);
	int getNumAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2) const;
	void changeNumAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2, int iChange);
	int getNumBonuses(BonusTypes eBonus) const;
	int getNumTotalBonuses() const;
	void changeNumBonuses(BonusTypes eBonus, int iChange);
	int getNumImprovements(ImprovementTypes eImprovement) const;
	void changeNumImprovements(ImprovementTypes eImprovement, int iChange);
	bool isEuropePlayer() const;	// TAC - AI Explore from Ship endless loop fix - koma13
	bool isIsland() const;

protected:
	int m_iID;
	int m_iNumTiles;
	int m_iNumOwnedTiles;
	int m_iNumRiverEdges;
	int m_iNumUnits;
	int m_iNumCities;
	int m_iNumStartingPlots;
	bool m_bWater;
	EnumMap<PlayerTypes, int> m_em_iUnitsPerPlayer;
	EnumMap<PlayerTypes, int> m_em_iCitiesPerPlayer;
	EnumMap<PlayerTypes, int> m_em_iPopulationPerPlayer;
	EnumMap<PlayerTypes, int> m_em_iPower;
	EnumMap<PlayerTypes, int> m_em_iBestFoundValue;
	EnumMap<TeamTypes  , int> m_em_iNumRevealedTiles;
	EnumMap<TeamTypes  , AreaAITypes> m_em_eAreaAIType;
	IDInfo* m_aTargetCities;
	EnumMap<PlayerTypes, EnumMap<YieldTypes , short> > m_em2_iYieldRateModifier;
	EnumMap<PlayerTypes, EnumMap<UnitAITypes, int  > > m_em2_iNumTrainAIUnits;
	EnumMap<PlayerTypes, EnumMap<UnitAITypes, int  > > m_em2_iNumAIUnits;
	EnumMap<BonusTypes, int> m_em_iNumBonuses;
	EnumMap<ImprovementTypes, int> m_em_iNumImprovements;
public:
	void read(CvSavegameReader reader);
	void write(CvSavegameWriter writer);
};

inline int  CvArea :: getID()                                   const { return m_iID; }
inline int  CvArea :: getNumTiles()                             const { return m_iNumTiles; }
inline bool CvArea :: isLake()                                  const { return ( isWater() && (getNumTiles()  <=  GC.getLAKE_MAX_AREA_SIZE() ) ); } // should this be changed now because of Terrain Lake?
inline int  CvArea :: getNumOwnedTiles()                        const { return m_iNumOwnedTiles; }
inline int  CvArea :: getNumUnownedTiles()                      const { return ( getNumTiles() - getNumOwnedTiles() ); }
inline int  CvArea :: getNumRiverEdges()                        const { return m_iNumRiverEdges; }
inline int  CvArea :: getNumUnits()                             const { return m_iNumUnits; }
inline int  CvArea :: getNumCities()                            const { return m_iNumCities; }
inline int  CvArea :: getNumStartingPlots()                     const { return m_iNumStartingPlots; }
inline bool CvArea :: isWater()                                 const { return m_bWater; }
inline int  CvArea :: getNumUnrevealedTiles(TeamTypes eIndex)   const { return ( getNumTiles() - getNumRevealedTiles(eIndex) ); }
inline bool CvArea :: isIsland()								const { return getNumTiles() < NUM_CITY_PLOTS_2_PLOTS; }



#endif
