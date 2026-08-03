/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/fontenum.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/fontenum.h"

#include "wx/arrstr.h"

#include <emscripten.h>

bool wxFontEnumerator::EnumerateFacenames(wxFontEncoding WXUNUSED(encoding), bool fixedWidthOnly)
{
    // The browser sandbox doesn't allow enumerating the installed system
    // fonts. As a best-effort, always report the generic CSS font families,
    // which every browser maps to some real font, plus the families known to
    // document.fonts (FontFaceSet), which only contains the fonts explicitly
    // loaded by the page or already used by it.
    wxArrayString facenames;

    static const char* const genericFamilies[] =
    {
        "serif",
        "sans-serif",
        "monospace",
        "cursive",
        "fantasy",
    };

    for ( size_t i = 0; i < WXSIZEOF(genericFamilies); i++ )
        facenames.push_back(wxString::FromUTF8(genericFamilies[i]));

    // Collect the unique families of document.fonts as a '\n'-separated
    // list (family names may be quoted in the FontFace API; the quotes are
    // stripped in C++ below to avoid quote characters inside EM_ASM).
    char* fonts = (char*)EM_ASM_INT({
        if (typeof document.fonts === 'undefined' || !document.fonts.forEach)
            return 0;
        var seen = {};
        var list = [];
        document.fonts.forEach(function(face) {
            var family = face.family;
            if (!seen[family]) {
                seen[family] = true;
                list.push(family);
            }
        });
        var str = list.join('\n');
        var len = lengthBytesUTF8(str) + 1;
        var buf = Module._malloc(len);
        stringToUTF8(str, buf, len);
        return buf;
    });

    if ( fonts )
    {
        const wxString list = wxString::FromUTF8(fonts);
        free(fonts);

        wxString family;
        for ( wxString::const_iterator it = list.begin(); ; ++it )
        {
            if ( it == list.end() || *it == '\n' )
            {
                // Strip the surrounding quotes the FontFace API may add to
                // family names containing spaces.
                if ( family.length() >= 2 &&
                        (family[0] == '"' || family[0] == '\'') &&
                        family.Last() == family[0] )
                    family = family.Mid(1, family.length() - 2);

                if ( !family.empty() )
                    facenames.push_back(family);
                family.clear();
                if ( it == list.end() )
                    break;
            }
            else
            {
                family += *it;
            }
        }
    }

    for ( size_t i = 0; i < facenames.size(); i++ )
    {
        if ( fixedWidthOnly &&
                facenames[i].Lower().Find("mono") == wxNOT_FOUND )
            continue;

        OnFacename(facenames[i]);
    }

    return true;
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& WXUNUSED(facename))
{
    return false;
}

#ifdef wxHAS_UTF8_FONTS
bool wxFontEnumerator::EnumerateEncodingsUTF8(const wxString& facename)
{
    return false;
}
#endif
