Enhanced Canals + Deep Canals + Canal Irrigation - Test Package
================================================================

This package adds deep canals, caps canal cost scaling at 4x, and
adds canal irrigation (freshwater canals create flood plains on desert).
Apply on top of a stock WTP 4.2.1 build.

CHANGES:
- Canal chains now cap cost scaling at 4x (was unbounded 2^n)
- New "Deep Canal" improvement: upgrade an existing canal to allow
  lake-capable ships (sloops, schooners, smuggling ships, etc.)
- Water area bridge graph connects lakes to oceans via deep canals
- Canal irrigation: canals connected to freshwater (lake, large river,
  or river) create flood plains on adjacent desert tiles
- Sea-only canals do NOT irrigate
- Updated Civilopedia text for canals and deep canals (EN/FR/DE/RU)

INSTALLATION:
1. Back up your existing files first!
2. Copy these files into your WTP mod folder, preserving paths:
   - CvGameCoreDLL.dll                  -> Assets/CvGameCoreDLL.dll
   - CIV4ArtDefines_Improvement.xml     -> Assets/XML/Art/CIV4ArtDefines_Improvement.xml
   - CIV4ImprovementInfos.xml           -> Assets/XML/Terrain/CIV4ImprovementInfos.xml
   - CIV4TerrainSchema.xml              -> Assets/XML/Terrain/CIV4TerrainSchema.xml
   - CIV4BuildInfos.xml                 -> Assets/XML/Units/CIV4BuildInfos.xml
   - XML_AUTO_UTF8_BuildInfo.xml        -> Assets/XML/Text/XML_AUTO_UTF8_BuildInfo.xml
   - XML_AUTO_UTF8_ImprovementInfo.xml  -> Assets/XML/Text/XML_AUTO_UTF8_ImprovementInfo.xml

TESTING:
- Build a canal adjacent to water, then upgrade it to a deep canal
- Verify sloops/schooners can traverse deep canals but galleons cannot
- Verify coastal ships still work on regular canals
- Build a canal chain (3+ tiles) and verify cost caps at 4x
- Connect a lake to the ocean via deep canals; verify routing works
- Build a canal from a lake adjacent to desert -> flood plains appear
- Build a multi-tile canal chain from lake through desert -> both sides irrigated
- Build a canal from ocean adjacent to desert -> NO flood plains
- Check Civilopedia entries for both canal types

Built from branch: feature/enhanced-canals (Assert/debug build)
