#include "EnigmaSerialization/Fonts/FontTTF.h"
#include "EnigmaSerialization/Utils.h"

#include <Enigma/Core/Core.h>

namespace Enigma::Serialization {
	namespace TTF {
		float FixedToFloatingPoint(Fixed v)
		{
			uint16_t intHalf = (v >> 16) & 0xFFFF;
			uint16_t fracHalf = v & 0xFFFF;

			float rslt = (float)intHalf + (1.0f / (float)fracHalf);

			return rslt;
		}
		float ShortToFixed2Dot14(short value)
		{
			uint16_t v = *(uint16_t*)&value;
			bool sign = v & BIT(15);
			uint8_t intElement = (v & BIT(14)) ? 1 : 0;
			uint16_t fracElement = v & 0x3FFF;

			float rslt = intElement + ((float)fracElement / (float)0x3FFF);
			if (sign) rslt *= -1;
			return rslt;
		}
		float ReadSFixed2Dot14(std::ifstream& file, bool littleE)
		{
			short value = ReadU16(file, littleE);
			return ShortToFixed2Dot14(value);
		}

		std::string FontTableDirectory::GetTagStr()
		{
			std::string str;
			str.resize(4);
			str[3] = (char)(tag & 0xFF);
			str[2] = (char)((tag >> 8) & 0xFF);
			str[1] = (char)((tag >> 16) & 0xFF);
			str[0] = (char)((tag >> 24) & 0xFF);
			return str;
		}

		void FontDirectory::Read(std::ifstream& file, FontDirectory& fontDirectory)
		{
			fontDirectory.scalerType    = ReadU32(file, false);
			fontDirectory.tableCount    = ReadU16(file, false);
			fontDirectory.searchRange   = ReadU16(file, false);
			fontDirectory.entrySelector = ReadU16(file, false);
			fontDirectory.rangeShift    = ReadU16(file, false);
		}
		void FontTableDirectory::Read(std::ifstream& file, FontDirectory& fontDirectory, std::map<std::string, FontTableDirectory>& tables)
		{
			// Loop through all table headers listed in the font directory
			for (int i = 0; i < fontDirectory.tableCount; ++i) {
				// Read in table header
				FontTableDirectory table;
				table.tag      = ReadU32(file, false);
				table.checkSum = ReadU32(file, false);
				table.offset   = ReadU32(file, false);
				table.length   = ReadU32(file, false);
				std::string tag = table.GetTagStr();
				// Store table
				tables.insert({ tag, table });
			}
		}

		void FontHead::Read(std::ifstream& file, FontHead& fontHead)
		{
			fontHead.version            = ReadU32(file, false);
			fontHead.fontRevision       = ReadU32(file, false);
			fontHead.checkSumAdjustment = ReadU32(file, false);
			fontHead.magicNumber        = ReadU32(file, false);
			fontHead.flags              = ReadU16(file, false);
			fontHead.unitsPerEm         = ReadU16(file, false);
			fontHead.created            = ReadU64(file, false);
			fontHead.modified           = ReadU64(file, false);
			fontHead.minX               = ReadS16(file, false);
			fontHead.minY               = ReadS16(file, false);
			fontHead.maxX               = ReadS16(file, false);
			fontHead.maxY               = ReadS16(file, false);
			fontHead.macStyle           = ReadU16(file, false);
			fontHead.lowestRecPPEM      = ReadU16(file, false);
			fontHead.fontDirectionHint  = ReadS16(file, false);
			fontHead.indexToLocFormat   = ReadS16(file, false);
			fontHead.glyphDataFormat    = ReadS16(file, false);
		}

		void HorizontalHeader::Read(std::ifstream& file, HorizontalHeader& horizontalHeader)
		{
			horizontalHeader.version             = ReadU32(file, false);
			horizontalHeader.ascent              = ReadS16(file, false);
			horizontalHeader.descent             = ReadS16(file, false);
			horizontalHeader.lineGap             = ReadS16(file, false);
			horizontalHeader.advanceWidthMax     = ReadU16(file, false);
			horizontalHeader.minLeftSideBearing  = ReadS16(file, false);
			horizontalHeader.minRightSideBearing = ReadS16(file, false);
			horizontalHeader.xMaxExtent          = ReadS16(file, false);
			horizontalHeader.caretSlopeRise      = ReadS16(file, false);
			horizontalHeader.caretSlopeRun       = ReadS16(file, false);
			horizontalHeader.caretOffset         = ReadS16(file, false);
			// reserved a.k.a. all 0s
			ReadU16(file, false);
			ReadU16(file, false);
			ReadU16(file, false);
			ReadU16(file, false);
			horizontalHeader.metricDataFormat    = ReadS16(file, false);
			horizontalHeader.numOfLongHorMetrics = ReadU16(file, false);
		}
		void HorizontalMetrics::Read(std::ifstream& file, HorizontalMetrics& horizontalMetrics, HorizontalHeader& horizontalHeader, MaxComponentElements& maxComponentElements)
		{
			horizontalMetrics.horizontalMetrics.resize(horizontalHeader.numOfLongHorMetrics);
			horizontalMetrics.leftSideBearing.resize(maxComponentElements.glyphCount - horizontalHeader.numOfLongHorMetrics);
			
			for (int i = 0; i < horizontalHeader.numOfLongHorMetrics; ++i) {
				horizontalMetrics.horizontalMetrics[i].advanceWidth    = ReadU16(file, false);
				horizontalMetrics.horizontalMetrics[i].leftSideBearing = ReadS16(file, false);
			}
			for (int i = 0; i < horizontalHeader.numOfLongHorMetrics - horizontalHeader.numOfLongHorMetrics; ++i) {
				horizontalMetrics.leftSideBearing[i] = ReadS16(file, false);
			}
		}

		void MaxComponentElements::Read(std::ifstream& file, MaxComponentElements& maxComponentElements)
		{
			maxComponentElements.version              = ReadU32(file, false);
			maxComponentElements.glyphCount           = ReadU16(file, false);
			maxComponentElements.maxPoints            = ReadU16(file, false);
			maxComponentElements.maxContours          = ReadU16(file, false);
			maxComponentElements.maxComponentPoints   = ReadU16(file, false);
			maxComponentElements.maxComponentContours = ReadU16(file, false);
			maxComponentElements.maxZones             = ReadU16(file, false);
			maxComponentElements.maxTwilightPoints    = ReadU16(file, false);
			maxComponentElements.maxStorage           = ReadU16(file, false);
			maxComponentElements.maxFunctionDefs      = ReadU16(file, false);
			maxComponentElements.maxInstructionDefs   = ReadU16(file, false);
			maxComponentElements.maxStackElements     = ReadU16(file, false);
			maxComponentElements.maxSizeOfIntructions = ReadU16(file, false);
			maxComponentElements.maxComponentElements = ReadU16(file, false);
			maxComponentElements.maxComponentDepth    = ReadU16(file, false);
		}

		void GlyphLocationTable::Read(std::ifstream& file, uint16_t glyphCount, FontHead& fontHead, GlyphLocationTable& glyphLocationTable)
		{
			// Reserve the memory needed
			glyphLocationTable.offsets.resize(glyphCount);
			glyphLocationTable.lengths.resize(glyphCount);
			// Loop through each glyph entry
			for (int i = 0; i < glyphCount; ++i) {
				// Read offset
				uint32_t offset;
				if (fontHead.indexToLocFormat == 1) {
					offset = ReadU32(file, false);
				}
				else {
					offset = (uint32_t)ReadU16(file, false);
					offset *= 2;
				}
				// Store offset and length data
				glyphLocationTable.offsets[i] = offset;
				if (i != 0) {
					uint32_t lastOffsetId = i - 1;
					uint32_t lastOffset = glyphLocationTable.offsets[lastOffsetId];
					glyphLocationTable.lengths[lastOffsetId] = offset - lastOffset;
				}
			}
		}

		void CharMapFormat0::Read(std::ifstream& file, CharMapFormat0* charMapFormat0)
		{
			charMapFormat0->length   = ReadU16(file, false);
			charMapFormat0->language = ReadU16(file, false);

			charMapFormat0->glyphIndexArray.resize(256);
			for (int i = 0; i < 256; ++i) {
				charMapFormat0->glyphIndexArray[i] = ReadU8(file);
			}
		}
		void CharMapFormat4::Read(std::ifstream& file, uint16_t glyphCount, CharMapFormat4* charMapFormat4)
		{
			charMapFormat4->length        = ReadU16(file, false);
			charMapFormat4->language      = ReadU16(file, false);
			charMapFormat4->segCountX2    = ReadU16(file, false);
			charMapFormat4->searchRange   = ReadU16(file, false);
			charMapFormat4->entrySelector = ReadU16(file, false);
			charMapFormat4->rangeShift    = ReadU16(file, false);

			uint32_t segCount = charMapFormat4->segCountX2 / 2;

			charMapFormat4->endCodes.resize(segCount);
			for (uint32_t i = 0; i < segCount; ++i) {
				charMapFormat4->endCodes[i] = ReadU16(file, false);
			}

			charMapFormat4->reservedPad = ReadU16(file, false);

			charMapFormat4->startCodes.resize(segCount);
			for (uint32_t i = 0; i < segCount; ++i) {
				charMapFormat4->startCodes[i] = ReadU16(file, false);
			}

			charMapFormat4->idDeltas.resize(segCount);
			for (uint32_t i = 0; i < segCount; ++i) {
				charMapFormat4->idDeltas[i] = ReadU16(file, false);
			}

			charMapFormat4->idRangeOffsets.resize(segCount);
			for (uint32_t i = 0; i < segCount; ++i) {
				charMapFormat4->idRangeOffsets[i] = ReadU16(file, false);
			}

			charMapFormat4->glyphIndexArray.resize(glyphCount);
			for (uint32_t i = 0; i < glyphCount; ++i) {
				charMapFormat4->glyphIndexArray[i] = ReadU16(file, false);
			}
		}
		void CharMapFormat12::Read(std::ifstream& file, uint16_t glyphCount, CharMapFormat12* charMapFormat12)
		{
			ReadU16(file, false); // skip reserved
			charMapFormat12->length   = ReadU32(file, false);
			charMapFormat12->language = ReadU32(file, false);
			charMapFormat12->nGroups  = ReadU32(file, false);

			charMapFormat12->groups.resize(charMapFormat12->nGroups);
			for (int i = 0; i < charMapFormat12->nGroups; ++i) {
				CharMapFormat12::Group& group = charMapFormat12->groups[i];
				group.startCharCode  = ReadU32(file, false);
				group.endCharCode    = ReadU32(file, false);
				group.startGlyphCode = ReadU32(file, false);
			}
		}
		void CharMapSubTable::Read(std::ifstream& file, CharMapSubTable& charMapSubTable)
		{
			charMapSubTable.platformID         = ReadU16(file, false);
			charMapSubTable.platformSpecificID = ReadU16(file, false);
			charMapSubTable.offset             = ReadU32(file, false);
		}
		void CharMapTable::Read(std::ifstream& file, uint16_t glyphCount, CharMapTable& charMapTable)
		{
			uint32_t charMapOffset = file.tellg();

			// Load header
			charMapTable.version = ReadU16(file, false);
			charMapTable.subTableCount = ReadU16(file, false);

			charMapTable.subTables.resize(charMapTable.subTableCount);

			// Read sub table headers
			for (int i = 0; i < charMapTable.subTableCount; ++i) {
				CharMapSubTable::Read(file, charMapTable.subTables[i]);
			}

			// Read sub table format
			for (int i = 0; i < charMapTable.subTableCount; ++i) {
				// Go to format data in file
				CharMapSubTable& subTable = charMapTable.subTables[i];
				uint32_t offset = charMapOffset + subTable.offset;
				file.seekg(offset);

				// Load the correct format
				subTable.format = ReadU16(file, false);
				switch (subTable.format)
				{
				case 0: {
					CharMapFormat0* format = new CharMapFormat0;
					CharMapFormat0::Read(file, format);
					subTable.formatTable = (void*)format;
				} break;
				case 4: {
					CharMapFormat4* format = new CharMapFormat4;
					CharMapFormat4::Read(file, glyphCount, format);
					subTable.formatTable = (void*)format;
				} break;
				case 12: {
					CharMapFormat12* format = new CharMapFormat12;
					CharMapFormat12::Read(file, glyphCount, format);
					subTable.formatTable = (void*)format;
				} break;
				default: {
					subTable.bestFitScore = -1000;
					LOG_WARNING("Unknown character map subtable format ( %u )", (uint32_t)subTable.format);
				}
				}

				// Find best fit score
				CalcBestFitScore(subTable);
			}

			// Find best subtable based on its best fit score
			int highestScore = 0;
			for (int i = 0; i < charMapTable.subTableCount; ++i) {
				int currentScore = charMapTable.subTables[i].bestFitScore;
				if (currentScore > highestScore) {
					highestScore = currentScore;
					charMapTable.bestFit = i;
				}
			}
		}
		void CharMapTable::CalcBestFitScore(CharMapSubTable& subtable)
		{
			subtable.bestFitScore += s_PlatformPriority[subtable.platformID];
			switch (subtable.platformID)
			{
			case 0: subtable.bestFitScore += s_UnicodePriority[subtable.platformSpecificID]; break;
			case 3: subtable.bestFitScore += s_MicrosoftPriority[subtable.platformSpecificID]; break;
			}
		}

		void GlyphData::ReadOffsets(std::ifstream& file, GlyphData& data, bool xOffset)
		{
			// Configure some stuff based on if we are reading a xOffset or not
			uint8_t vectorBit = BIT(1 + (xOffset ? 0 : 1));
			uint8_t signBit = BIT(4 + (xOffset ? 0 : 1));
			std::vector<int>& offsets = (xOffset ? data.offsetsX : data.offsetsY);

			offsets.resize(data.offsetCount);
			offsets[0] = 0;
			for (int i = 0; i < data.offsetCount; ++i) {
				uint8_t flag = data.offsetFlags[i];
				offsets[i] = offsets[std::max(0, i - 1)];
				// Check if x is a signed or unsigned int
				if (flag & vectorBit) {
					uint8_t x = ReadU8(file);
					// See if x is positive or negative
					if (flag & signBit) offsets[i] += x;
					else offsets[i] -= x;
				}
				else {
					// See if the offset exists or not
					if (flag & signBit) continue;
					// store offset
					short x = ReadS16(file, false);
					offsets[i] += x;
				}
			}
		}
		void GlyphData::ReadCompound(std::ifstream& file, GlyphData& data, uint32_t glyphTableOffset, GlyphLocationTable& glyphLocationTable)
		{
			data.numberOfContours = 0;
			bool lastGlyph = false;
			while (!lastGlyph) {
				uint16_t flags      = ReadU16(file, false);
				uint16_t glyphIndex = ReadU16(file, false);

				// Read glyph data at glyph index
				uint32_t compoundDescIndex = file.tellg();
				uint32_t glyphOffset = glyphTableOffset + glyphLocationTable.offsets[glyphIndex];

				file.seekg(glyphOffset);
				GlyphData component;
				GlyphData::Read(file, component, glyphTableOffset, glyphLocationTable);
				file.seekg(compoundDescIndex);

				// Read arguments 1 & 2
				bool argsAreWords = flags & BIT(0);
				bool argsAreXY    = flags & BIT(1);
				float xOffset = 0;
				float yOffset = 0;
				if (argsAreXY) {
					xOffset = argsAreWords ? ReadS16(file, false) : (short)ReadS8(file);
					yOffset = argsAreWords ? ReadS16(file, false) : (short)ReadS8(file);
				}
				else if (!argsAreXY) {
					short compoundIndex  = argsAreWords ? ReadS16(file, false) : (short)ReadS8(file);
					short componentIndex = argsAreWords ? ReadS16(file, false) : (short)ReadS8(file);

					// TODO: figure out what needs to happen here
				}

				// Read scale matrix
				float xScale  = 1; // a
				float scale01 = 0; // b
				float scale10 = 0; // c
				float yScale  = 1; // d
				if (flags & BIT(3)) {
					xScale = ReadSFixed2Dot14(file, false);
					yScale = xScale;
				}
				else if (flags & BIT(6)) {
					xScale = ReadSFixed2Dot14(file, false);
					yScale = ReadSFixed2Dot14(file, false);
				}
				else if (flags & BIT(7)) {
					xScale  = ReadSFixed2Dot14(file, false);
					scale01 = ReadSFixed2Dot14(file, false);
					scale10 = ReadSFixed2Dot14(file, false);
					yScale  = ReadSFixed2Dot14(file, false);
				}

				// Transform all points in the component glyph using the 2x2 scale matrix and offset values
				float m = std::max(abs(xScale), abs(scale01));
				float n = std::max(abs(scale10), abs(yScale));
				float comp = 33.0f / 65536.0f;
				if (abs(abs(xScale) - abs(scale10)) <= comp) m *= 2;
				if (abs(abs(scale01) - abs(yScale)) <= comp) n *= 2;

				for (int i = 0; i < component.offsetCount; ++i) {
					component.offsetsX[i] = m * ((xScale  / m) * component.offsetsX[i] + (scale10 / m) * component.offsetsY[i] + xOffset);
					component.offsetsY[i] = n * ((scale01 / n) * component.offsetsX[i] + (yScale  / n) * component.offsetsY[i] + yOffset);
				}

				uint32_t indexOffset = data.offsetCount;
				for (int i = 0; i < component.endPointsOfContours.size(); ++i) {
					component.endPointsOfContours[i] += indexOffset;
				}
				
				data.offsetCount += component.offsetCount;
				data.numberOfContours += component.numberOfContours;
				data.offsetsX.insert(data.offsetsX.end(), component.offsetsX.begin(), component.offsetsX.end());
				data.offsetsY.insert(data.offsetsY.end(), component.offsetsY.begin(), component.offsetsY.end());
				data.offsetFlags.insert(data.offsetFlags.end(), component.offsetFlags.begin(), component.offsetFlags.end());
				data.endPointsOfContours.insert(data.endPointsOfContours.end(), component.endPointsOfContours.begin(), component.endPointsOfContours.end());

				// Check if there are more glyphs following this one
				lastGlyph = !(flags & BIT(5));
			}
		}
		void GlyphData::ReadSimple(std::ifstream& file, GlyphData& data)
		{
			data.endPointsOfContours.resize(data.numberOfContours);
			for (int i = 0; i < data.numberOfContours; ++i) {
				data.endPointsOfContours[i] = ReadU16(file, false);
			}

			data.offsetCount = data.endPointsOfContours[data.endPointsOfContours.size() - 1] + 1;
			data.offsetFlags.resize(data.offsetCount);

			// Read instructions
			uint16_t instructionCount = ReadU16(file, false);
			data.instructions.resize(instructionCount);
			for (uint16_t i = 0; i < instructionCount; ++i) {
				data.instructions[i] = ReadU8(file);
			}

			// Read flags
			for (int i = 0; i < data.offsetCount; ++i) {
				uint8_t flag = ReadU8(file);
				data.offsetFlags[i] = flag;
				if (flag & BIT(3)) {
					uint8_t repeatCount = ReadU8(file);
					for (int j = 0; j < repeatCount; ++j) {
						if (i == data.offsetFlags.size() - 1) break;
						data.offsetFlags[++i] = flag;
					}
				}
			}

			// Read X values
			GlyphData::ReadOffsets(file, data, true);

			// Read Y values
			GlyphData::ReadOffsets(file, data, false);
		}
		void GlyphData::Read(std::ifstream& file, GlyphData& data, uint32_t glyphTableOffset, GlyphLocationTable& glyphLocationTable)
		{
			// Read glyph description
			data.numberOfContours = ReadS16(file, false);
			data.minX = ReadS16(file, false);
			data.minY = ReadS16(file, false);
			data.maxX = ReadS16(file, false);
			data.maxY = ReadS16(file, false);

			// Read everything else
			data.isCompound = (data.numberOfContours <= 0);
			if (data.isCompound) GlyphData::ReadCompound(file, data, glyphTableOffset, glyphLocationTable);
			else GlyphData::ReadSimple(file, data);
		}	

		uint16_t CharMapSubTable::Search(uint32_t unicode) {
			switch (format)
			{
			case 0:  return ((CharMapFormat0*)formatTable)->Search(unicode);
			case 4:  return ((CharMapFormat4*)formatTable)->Search(unicode);
			case 12: return ((CharMapFormat12*)formatTable)->Search(unicode);
			default: LOG_WARNING("Unknown character map subtable format ( %u )", (uint32_t)format);
			}
			return (uint16_t)-1;
		}
		uint16_t CharMapFormat0::Search(uint32_t unicode)
		{
			if (unicode >= (uint8_t)-1) return 0;
			return glyphIndexArray[unicode];
		}
		uint16_t CharMapFormat4::Search(uint32_t unicode)
		{
			if (unicode >= (uint16_t)-1) return 0;
			uint16_t firstIndex = (uint16_t)-1;
			for (uint16_t i = 0; i < segCountX2 / 2; ++i) {
				uint16_t endCode = endCodes[i];
				if (endCode == 0xFFFF) {
					return 0; // return missing char glyph id
				}
				if (endCode >= unicode) {
					firstIndex = i;
					break;
				}
			}
			if (firstIndex == (uint16_t)-1) {
				return 0; // return missing char glyph id
			}

			uint16_t startCode = startCodes[firstIndex];
			if (startCode <= unicode) {
				uint16_t idDelta = idDeltas[firstIndex];
				uint16_t idRangeOffset = idRangeOffsets[firstIndex];

				if (idRangeOffset == 0) {
					uint16_t glyphID = idDelta + unicode;
					return glyphID;
				}

				// Convert pointer stuff to a index into the glyph index array
				uint16_t index = (idRangeOffset / 2) + (unicode - startCode) + (firstIndex - segCountX2 / 2);
				uint16_t glyphIndex = glyphIndexArray[index];
				if (glyphIndex == 0) return 0;

				uint16_t glyphID = idDelta + glyphIndex;
				return glyphID;
			}

			return 0; // return missing char glyph id
		}
		uint16_t CharMapFormat12::Search(uint32_t unicode)
		{
			uint32_t groupIndex = (uint32_t)-1;
			for (int i = 0; i < nGroups; ++i) {
				CharMapFormat12::Group& group = groups[i];
				if (unicode >= group.startCharCode && unicode <= group.endCharCode) {
					groupIndex = i;
					break;
				}
			}
			if (groupIndex == (uint32_t)-1) return 0;
			CharMapFormat12::Group& group = groups[groupIndex];

			uint16_t glyphID = group.startGlyphCode + (unicode - group.startCharCode);
			return glyphID;
		}

		FontTTF::FontTTF(const std::string& filename)
		{
			// Open file
			std::ifstream file;
			file.open(filename, std::ios::binary);
			if (!file.is_open()) {
				LOG_SOFT_ERROR("Failed to open font file ( %s )", filename.c_str());
				return;
			}

			// Read tables
			FontDirectory::Read(file, m_FontDirectory);
			FontTableDirectory::Read(file, m_FontDirectory, m_Tables);

			// Read font head table
			GoToTable(file, "head");
			FontHead::Read(file, m_FontHead);

			// Read max component elements table
			GoToTable(file, "maxp");
			MaxComponentElements::Read(file, m_MaxComponentElements);

			// Read font horizontal header table
			GoToTable(file, "hhea");
			HorizontalHeader::Read(file, m_HorizontalHeader);

			// Read font horizontal metric table
			GoToTable(file, "hmtx");
			HorizontalMetrics::Read(file, m_HorizontalMetrics, m_HorizontalHeader, m_MaxComponentElements);

			// Read glyph location table
			GoToTable(file, "loca");
			GlyphLocationTable::Read(file, m_MaxComponentElements.glyphCount, m_FontHead, m_GlyphLocationTable);

			// Read character map table
			GoToTable(file, "cmap");
			CharMapTable::Read(file, m_MaxComponentElements.glyphCount, m_CharMapTable);

			// Read glyphs
			uint32_t glyphTableOffset = m_Tables["glyf"].offset;
			m_GlyphData.resize(m_MaxComponentElements.glyphCount);
			for (int i = 0; i < m_MaxComponentElements.glyphCount - 1; ++i) {
				// Get offset to glyph data
				uint32_t glyphOffset = glyphTableOffset + m_GlyphLocationTable.offsets[i];
				file.seekg(glyphOffset);

				// Read glyph
				GlyphData::Read(file, m_GlyphData[i], glyphTableOffset, m_GlyphLocationTable);
			}

			file.close();
		}
		FontTTF::~FontTTF()
		{
		}

		Renderer::Glyph FontTTF::GetGlyph(uint16_t unicode)
		{
			// Get raw glyph data
			CharMapSubTable& subTable = m_CharMapTable.subTables[m_CharMapTable.bestFit];
			uint16_t glyphID = subTable.Search(unicode);
			GlyphData& data = m_GlyphData[glyphID];
			if (data.numberOfContours == 0) data = m_GlyphData[0];

			// Calculate glyph data
			Renderer::Glyph glyph;
			
			// Get horizontal advance data
			uint32_t metricIndex = 0;
			if (glyphID >= m_HorizontalHeader.numOfLongHorMetrics) metricIndex = m_HorizontalHeader.numOfLongHorMetrics - 1;
			else if (glyphID < m_HorizontalHeader.numOfLongHorMetrics) metricIndex = glyphID;
			glyph.horizontalAdvance = m_HorizontalMetrics.horizontalMetrics[metricIndex].advanceWidth;

			// Store min and max points
			glyph.boundingBox.min = { (float)data.minX, (float)data.minY };
			glyph.boundingBox.max = { (float)data.maxX, (float)data.maxY };
			// Find the position which is the midpoint between the min and max points
			glyph.boundingBox.position = {
				(glyph.boundingBox.min.x + glyph.boundingBox.max.x) / 2.0f,
				(glyph.boundingBox.min.y + glyph.boundingBox.max.y) / 2.0f
			};
			// Find the size. I don't know how to word the way this works, it just does
			glyph.boundingBox.size = {
				glm::distance(glyph.boundingBox.min.x, glyph.boundingBox.max.x) / 2.0f,
				glm::distance(glyph.boundingBox.min.y, glyph.boundingBox.max.y) / 2.0f
			};

			// Loop through each contour
			uint16_t startPoint = 0;
			int impliedPointCount = 0;
			for (int contourIndex = 0; contourIndex < data.numberOfContours; ++contourIndex) {
				uint16_t endPoint = data.endPointsOfContours[contourIndex];

				bool lastOnCurve = true;
				int startImpliedPointCount = impliedPointCount;

				// Loop through each point in the current contour
				for (int i = startPoint; i <= endPoint; ++i) {
					// Get the point
					glm::vec2 point = {
						data.offsetsX[i],
						data.offsetsY[i]
					};

					// Handle implied points
					// An implied point is the point directly between two control points
					bool onCurve = (data.offsetFlags[i] & BIT(0));
					if (((onCurve && lastOnCurve) || (!onCurve && !lastOnCurve)) && i != startPoint) {
						// Get implied point
						uint32_t lastPointIndex = i + impliedPointCount - 1;
						glm::vec2 lastPoint = glyph.points[lastPointIndex].position;
						glm::vec2 impliedPoint = {
							(point.x + lastPoint.x) / 2.0f,
							(point.y + lastPoint.y) / 2.0f,
						};
						// Store implied point
						glyph.points.push_back({ impliedPoint, Renderer::TextPointType::TextPointType_OnCurve | Renderer::TextPointType::TextPointType_Implied });
						impliedPointCount += 1;
					}
					lastOnCurve = onCurve;

					// Store point
					uint8_t flags = (i == startPoint) ? Renderer::TextPointType::TextPointType_StartOfContour : 0;
					flags |= (onCurve ? Renderer::TextPointType::TextPointType_OnCurve : Renderer::TextPointType::TextPointType_Control);
					glyph.points.push_back({ point, flags });
				}
				// Create an extra implied point if the first and last points are on the curve
				bool firstOnCurve = (data.offsetFlags[startPoint] & BIT(0));
				if ((lastOnCurve && firstOnCurve) || (!lastOnCurve && !firstOnCurve)) {
					uint32_t firstPointIndex = startPoint + startImpliedPointCount;
					uint32_t lastPointIndex = endPoint + impliedPointCount;
					glm::vec2 firstPoint = glyph.points[firstPointIndex].position;
					glm::vec2 lastPoint = glyph.points[lastPointIndex].position;
					glm::vec2 impliedPoint = {
						(firstPoint.x + lastPoint.x) / 2.0f,
						(firstPoint.y + lastPoint.y) / 2.0f,
					};
					glyph.points.push_back({ impliedPoint, Renderer::TextPointType::TextPointType_OnCurve | Renderer::TextPointType::TextPointType_Implied });
					impliedPointCount += 1;
				}

				// Store contour
				Renderer::GlyphContour contour;
				contour.start = startPoint + startImpliedPointCount;
				contour.end = endPoint + impliedPointCount + 1;
				glyph.contours.push_back(contour);

				startPoint = endPoint + 1;
			}

			return glyph;
		}
		int FontTTF::GetVerticalSpacing()
		{
			return m_HorizontalHeader.ascent - m_HorizontalHeader.descent + m_HorizontalHeader.lineGap;
		}
		int FontTTF::GetSpaceHorizontalAdvance()
		{
			return m_HorizontalHeader.advanceWidthMax;
		}

		void FontTTF::GoToTable(std::ifstream& file, const std::string& table)
		{
			// Goto the offset specified in the specified table. 
			// All file read functions will start at the offset after this.
			file.seekg(m_Tables[table].offset);
		}			
	}
}