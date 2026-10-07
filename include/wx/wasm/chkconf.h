/*
 * Name:        wx/wasm/chkconf.h
 * Purpose:     wxWasm-specific config settings consistency checks
 * Author:      Hugo Armando Castellanos Morales
 * Created:     2026-10-05
 * Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
 * Licence:     wxWindows licence
 */

/* THIS IS A C FILE, DON'T USE C++ FEATURES (IN PARTICULAR COMMENTS) IN IT */

#if wxUSE_METAFILE
#   undef wxUSE_METAFILE
#   define wxUSE_METAFILE 0
#endif /* wxUSE_METAFILE */
