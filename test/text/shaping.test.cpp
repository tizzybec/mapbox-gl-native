
#include <mbgl/test/util.hpp>

#include <mbgl/text/bidi.hpp>
#include <mbgl/text/tagged_string.hpp>
#include <mbgl/text/shaping.hpp>
#include <mbgl/util/constants.hpp>

using namespace mbgl;
using namespace util;

TEST(Shaping, ZWSP) {
    Glyph glyph;
    glyph.id = u'中';
    glyph.metrics.width = 18;
    glyph.metrics.height = 18;
    glyph.metrics.left = 2;
    glyph.metrics.top = -8;
    glyph.metrics.advance = 21;

    auto immutableGlyph = Immutable<Glyph>(makeMutable<Glyph>(std::move(glyph)));
    std::vector<std::string> fontStack{{"font-stack"}};
    GlyphMap glyphs = {
        { FontStackHasher()(fontStack), {{u'中', immutableGlyph}} }
    };

    BiDi bidi;
    TaggedString string(u"中中\u200b中中\u200b中中\u200b中中中中中中\u200b中中", SectionOptions(1.0f, fontStack));
    Shaping shaping = getShaping(string,
                                 5 * ONE_EM,   // maxWidth
                                 ONE_EM,       // lineHeight
                                 style::SymbolAnchorType::Center,
                                 style::TextJustifyType::Center,
                                 0,            // spacing
                                 {0.0f, 0.0f}, // translate
                                 WritingModeType::Horizontal,
                                 bidi,
                                 glyphs
                                 );

    ASSERT_EQ(shaping.lineCount, 3);
    ASSERT_EQ(shaping.top, -36);
    ASSERT_EQ(shaping.bottom, 36);
    ASSERT_EQ(shaping.left, -63);
    ASSERT_EQ(shaping.right, 63);
    ASSERT_EQ(shaping.writingMode, WritingModeType::Horizontal);
}
