// Copyright 2017 Patrick Brosi
// info@patrickbrosi.de

#ifndef PFXML_H_
#define PFXML_H_

#include <fcntl.h>
#include <unistd.h>
#ifndef PFXML_NO_ZLIB
#include <zlib.h>
#endif

#ifndef PFXML_NO_BZLIB
#include <bzlib.h>
#endif

#include <cassert>
#include <cstring>
#include <fstream>
#include <map>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

namespace pfxml {

static const size_t BUFFER_S = 32 * 1024 * 1024;

enum state {
  NONE,
  IN_TAG_NAME,
  IN_TAG_NAME_META,
  IN_TAG,
  IN_TAG_CLOSE,
  IN_TAG_NAME_CLOSE,
  IN_TAG_TENTATIVE,
  IN_ATTRKEY,
  AFTER_ATTRKEY,
  AW_IN_ATTRVAL,
  IN_ATTRVAL_SQ,
  IN_ATTRVAL_DQ,
  IN_TEXT,
  IN_COMMENT_TENTATIVE,
  IN_COMMENT_TENTATIVE2,
  IN_COMMENT,
  IN_COMMENT_CL_TENTATIVE,
  IN_COMMENT_CL_TENTATIVE2,
  AW_CLOSING,
  WS_SKIP
};

// see
// http://en.wikipedia.org/wiki/List_of_XML_and_HTML_character_entity_references
static const std::map<std::string, const char*> ENTITIES = {
    {"aacute", "á"},
    {"Aacute", "Á"},
    {"acirc", "â"},
    {"Acirc", "Â"},
    {"acute", "´"},
    {"aelig", "æ"},
    {"AElig", "Æ"},
    {"agrave", "à"},
    {"Agrave", "À"},
    {"alefsym", "ℵ"},
    {"alpha", "α"},
    {"Alpha", "Α"},
    {"amp", "&"},
    {"and", "∧"},
    {"ang", "∠"},
    {"apos", "'"},
    {"aring", "å"},
    {"Aring", "Å"},
    {"asymp", "≈"},
    {"atilde", "ã"},
    {"Atilde", "Ã"},
    {"auml", "ä"},
    {"Auml", "Ä"},
    {"bdquo", "„"},
    {"beta", "β"},
    {"Beta", "Β"},
    {"brvbar", "¦"},
    {"bull", "•"},
    {"cap", "∩"},
    {"ccedil", "ç"},
    {"Ccedil", "Ç"},
    {"cedil", "¸"},
    {"cent", "¢"},
    {"chi", "χ"},
    {"Chi", "Χ"},
    {"circ", "ˆ"},
    {"clubs", "♣"},
    {"cong", "≅"},
    {"copy", "©"},
    {"crarr", "↵"},
    {"cup", "∪"},
    {"curren", "¤"},
    {"dagger", "†"},
    {"Dagger", "‡"},
    {"darr", "↓"},
    {"dArr", "⇓"},
    {"deg", "°"},
    {"delta", "δ"},
    {"Delta", "Δ"},
    {"diams", "♦"},
    {"divide", "÷"},
    {"eacute", "é"},
    {"Eacute", "É"},
    {"ecirc", "ê"},
    {"Ecirc", "Ê"},
    {"egrave", "è"},
    {"Egrave", "È"},
    {"empty", "∅"},
    {"emsp", "\xE2\x80\x83"},
    {"ensp", "\xE2\x80\x82"},
    {"epsilon", "ε"},
    {"Epsilon", "Ε"},
    {"equiv", "≡"},
    {"eta", "η"},
    {"Eta", "Η"},
    {"eth", "ð"},
    {"ETH", "Ð"},
    {"euml", "ë"},
    {"Euml", "Ë"},
    {"euro", "€"},
    {"exist", "∃"},
    {"fnof", "ƒ"},
    {"forall", "∀"},
    {"frac12", "½"},
    {"frac14", "¼"},
    {"frac34", "¾"},
    {"frasl", "⁄"},
    {"gamma", "γ"},
    {"Gamma", "Γ"},
    {"ge", "≥"},
    {"gt", ">"},
    {"harr", "↔"},
    {"hArr", "⇔"},
    {"hearts", "♥"},
    {"hellip", "…"},
    {"iacute", "í"},
    {"Iacute", "Í"},
    {"icirc", "î"},
    {"Icirc", "Î"},
    {"iexcl", "¡"},
    {"igrave", "ì"},
    {"Igrave", "Ì"},
    {"image", "ℑ"},
    {"infin", "∞"},
    {"int", "∫"},
    {"iota", "ι"},
    {"Iota", "Ι"},
    {"iquest", "¿"},
    {"isin", "∈"},
    {"iuml", "ï"},
    {"Iuml", "Ï"},
    {"kappa", "κ"},
    {"Kappa", "Κ"},
    {"lambda", "λ"},
    {"Lambda", "Λ"},
    {"lang", "〈"},
    {"laquo", "«"},
    {"larr", "←"},
    {"lArr", "⇐"},
    {"lceil", "⌈"},
    {"ldquo", "“"},
    {"le", "≤"},
    {"lfloor", "⌊"},
    {"lowast", "∗"},
    {"loz", "◊"},
    {"lrm", "\xE2\x80\x8E"},
    {"lsaquo", "‹"},
    {"lsquo", "‘"},
    {"lt", "<"},
    {"macr", "¯"},
    {"mdash", "—"},
    {"micro", "µ"},
    {"middot", "·"},
    {"minus", "−"},
    {"mu", "μ"},
    {"Mu", "Μ"},
    {"nabla", "∇"},
    {"nbsp", "\xC2\xA0"},
    {"ndash", "–"},
    {"ne", "≠"},
    {"ni", "∋"},
    {"not", "¬"},
    {"notin", "∉"},
    {"nsub", "⊄"},
    {"ntilde", "ñ"},
    {"Ntilde", "Ñ"},
    {"nu", "ν"},
    {"Nu", "Ν"},
    {"oacute", "ó"},
    {"Oacute", "Ó"},
    {"ocirc", "ô"},
    {"Ocirc", "Ô"},
    {"oelig", "œ"},
    {"OElig", "Œ"},
    {"ograve", "ò"},
    {"Ograve", "Ò"},
    {"oline", "‾"},
    {"omega", "ω"},
    {"Omega", "Ω"},
    {"omicron", "ο"},
    {"Omicron", "Ο"},
    {"oplus", "⊕"},
    {"or", "∨"},
    {"ordf", "ª"},
    {"ordm", "º"},
    {"oslash", "ø"},
    {"Oslash", "Ø"},
    {"otilde", "õ"},
    {"Otilde", "Õ"},
    {"otimes", "⊗"},
    {"ouml", "ö"},
    {"Ouml", "Ö"},
    {"para", "¶"},
    {"part", "∂"},
    {"permil", "‰"},
    {"perp", "⊥"},
    {"phi", "φ"},
    {"Phi", "Φ"},
    {"piv", "ϖ"},
    {"pi", "π"},
    {"Pi", "Π"},
    {"plusmn", "±"},
    {"pound", "£"},
    {"prime", "′"},
    {"Prime", "″"},
    {"prod", "∏"},
    {"prop", "∝"},
    {"psi", "ψ"},
    {"Psi", "Ψ"},
    {"quot", "\""},
    {"radic", "√"},
    {"rang", "〉"},
    {"raquo", "»"},
    {"rarr", "→"},
    {"rArr", "⇒"},
    {"rceil", "⌉"},
    {"rdquo", "”"},
    {"real", "ℜ"},
    {"reg", "®"},
    {"rfloor", "⌋"},
    {"rho", "ρ"},
    {"Rho", "Ρ"},
    {"rlm", "\xE2\x80\x8F"},
    {"rsaquo", "›"},
    {"rsquo", "’"},
    {"sbquo", "‚"},
    {"scaron", "š"},
    {"Scaron", "Š"},
    {"sdot", "⋅"},
    {"sect", "§"},
    {"shy", "\xC2\xAD"},
    {"sigmaf", "ς"},
    {"sigma", "σ"},
    {"Sigma", "Σ"},
    {"sim", "∼"},
    {"spades", "♠"},
    {"sub", "⊂"},
    {"sube", "⊆"},
    {"sum", "∑"},
    {"sup", "⊃"},
    {"sup1", "¹"},
    {"sup2", "²"},
    {"sup3", "³"},
    {"supe", "⊇"},
    {"szlig", "ß"},
    {"tau", "τ"},
    {"Tau", "Τ"},
    {"there4", "∴"},
    {"thetasym", "ϑ"},
    {"theta", "θ"},
    {"Theta", "Θ"},
    {"thinsp", "\xE2\x80\x89"},
    {"thorn", "þ"},
    {"THORN", "Þ"},
    {"tilde", "˜"},
    {"times", "×"},
    {"trade", "™"},
    {"uacute", "ú"},
    {"Uacute", "Ú"},
    {"uarr", "↑"},
    {"uArr", "⇑"},
    {"ucirc", "û"},
    {"Ucirc", "Û"},
    {"ugrave", "ù"},
    {"Ugrave", "Ù"},
    {"uml", "¨"},
    {"upsih", "ϒ"},
    {"upsilon", "υ"},
    {"Upsilon", "Υ"},
    {"uuml", "ü"},
    {"Uuml", "Ü"},
    {"weierp", "℘"},
    {"xi", "ξ"},
    {"Xi", "Ξ"},
    {"yacute", "ý"},
    {"Yacute", "Ý"},
    {"yen", "¥"},
    {"yuml", "ÿ"},
    {"Yuml", "Ÿ"},
    {"zeta", "ζ"},
    {"Zeta", "Ζ"},
    {"zwj", "\xE2\x80\x8D"},
    {"zwnj", "\xE2\x80\x8C"}};

class parse_exc : public std::exception {
 public:
  parse_exc(std::string msg, std::string file, const char* p, char* buff,
            size_t offset) {
    std::stringstream ss;
    ss << file << " at position " << (offset + (p - buff)) << ": " << msg;
    _msg = ss.str();
  }
  ~parse_exc() throw() {}

  virtual const char* what() const throw() { return _msg.c_str(); }

 private:
  std::string _msg;
};

struct parser_state {
  parser_state() : s(NONE), hanging(0), off(0) {}
  std::stack<std::string> tag_stack;
  state s;
  size_t hanging;
  int64_t off;
};

typedef std::vector<std::pair<const char*, const char*>> attr_map;

struct tag {
  const char* name;
  const char* text;
  pfxml::attr_map attrs;
  const char* attr(const char* k) const {
    for (const auto& kv : attrs) {
      if (strcmp(kv.first, k) == 0) return kv.second;
    }
    return 0;
  }
};

class file {
 public:
  file(const std::string& path);
  ~file();

  const tag& get() const;

  bool next();
  size_t level() const;
  void reset();
  parser_state state();
  void set_state(const parser_state& s);
  static std::string decode(const char* str);
  static std::string decode(const std::string& str);

 private:
  int _file;
#ifndef PFXML_NO_ZLIB
  gzFile _gzfile;
#endif

#ifndef PFXML_NO_BZLIB
  BZFILE* _bzfile;
#endif
  FILE* _bzfhandle = 0;
  parser_state _s;
  parser_state _prevs;
  char** _buf;
  char* _c;
  int64_t _last_bytes;

  const char* _tmp;
  const char* _tmp2;

  size_t _which;
  std::string _path;

  int64_t _tot_read_bef;
  int64_t _last_new_data;

  tag _ret;

  bool _gzip;
  bool _bzip;

  static size_t utf8(size_t cp, char* out);
  const char* empty_str = "";
};

// _____________________________________________________________________________
inline file::file(const std::string& path)
    : _file(0),
#ifndef PFXML_NO_ZLIB
      _gzfile(Z_NULL),
#endif
#ifndef PFXML_NO_BZLIB
      _bzfile(0),
#endif
      _c(0),
      _last_bytes(0),
      _which(0),
      _path(path),
      _tot_read_bef(0),
      _gzip(false),
      _bzip(false) {
  _buf = new char*[2];
  _buf[0] = new char[BUFFER_S + 1];
  _buf[1] = new char[BUFFER_S + 1];

  if (path.size() > 2 && path[path.size() - 1] == 'z' &&
      path[path.size() - 2] == 'g' && path[path.size() - 3] == '.') {
    _gzip = true;
  }

  if (path.size() > 3 && path[path.size() - 1] == '2' &&
      path[path.size() - 2] == 'z' && path[path.size() - 3] == 'b' &&
      path[path.size() - 4] == '.') {
    _bzip = true;
  }

  reset();
}

// _____________________________________________________________________________
inline file::~file() {
  delete[] _buf[0];
  delete[] _buf[1];
  delete[] _buf;
  if (_gzip) {
#ifndef PFXML_NO_ZLIB
    if (_gzfile != Z_NULL) {
      gzclose(_gzfile);
      _gzfile = Z_NULL;
    }
#endif
  } else if (_bzip) {
#ifndef PFXML_NO_BZLIB
    if (_bzfile) {
      int err;
      BZ2_bzReadClose(&err, _bzfile);
      _bzfile = nullptr;
    }
#endif
    if (_bzfhandle) fclose(_bzfhandle);
  } else {
    close(_file);
  }
}

// _____________________________________________________________________________
inline void file::reset() {
  _which = 0;
  _s.s = NONE;
  _s.hanging = 0;
  _tot_read_bef = 0;

  if (_file) {
    if (_gzip) {
#ifndef PFXML_NO_ZLIB
      if (_gzfile != Z_NULL) {
        gzclose(_gzfile);
        _gzfile = Z_NULL;
      }
#endif
    } else if (_bzip) {
#ifndef PFXML_NO_BZLIB
      if (_bzfile) {
        int err;
        BZ2_bzReadClose(&err, _bzfile);
        _bzfile = nullptr;
      }
      if (_bzfhandle) {
        fclose(_bzfhandle);
        _bzfhandle = 0;
      }
#endif
    } else {
      close(_file);
    }
  }

  if (_gzip) {
#ifndef PFXML_NO_ZLIB
    _gzfile = gzopen(_path.c_str(), "r");
    if (_gzfile == Z_NULL)
      throw parse_exc(std::string("could not open file"), _path, 0, 0, 0);
#else
    throw parse_exc(std::string("could not open gzip file, pfxml was compiled "
                                "without zlib support"),
                    _path, 0, 0, 0);
#endif
  } else if (_bzip) {
#ifndef PFXML_NO_BZLIB
    _bzfhandle = fopen(_path.c_str(), "r");
    int err;
    if (!_bzfhandle)
      throw parse_exc(std::string("could not open file"), _path, 0, 0, 0);

    _bzfile = BZ2_bzReadOpen(&err, _bzfhandle, 0, 0, NULL, 0);

    if (!_bzfile || err != BZ_OK) {
      throw parse_exc(std::string("could not read bzip file"), _path, 0, 0, 0);
    }
#else
    throw parse_exc(std::string("could not open bzip file, pfxml was compiled "
                                "without bzlib support"),
                    _path, 0, 0, 0);
#endif
  } else {
    _file = open(_path.c_str(), O_RDONLY);
    if (_file < 0)
      throw parse_exc(std::string("could not open file"), _path, 0, 0, 0);
  }

  if (!_gzip && !_bzip) {
#ifdef __unix__
    posix_fadvise(_file, 0, 0, POSIX_FADV_SEQUENTIAL);
#endif
  }

  if (_gzip) {
#ifndef PFXML_NO_ZLIB
    _last_bytes = gzread(_gzfile, _buf[_which], BUFFER_S);
#endif
  } else if (_bzip) {
#ifndef PFXML_NO_BZLIB
    int err;
    _last_bytes = BZ2_bzRead(&err, _bzfile, _buf[_which], BUFFER_S);
#endif
  } else {
    _last_bytes = read(_file, _buf[_which], BUFFER_S);
  }

  _last_new_data = _last_bytes;
  _c = _buf[_which];
  while (!_s.tag_stack.empty()) _s.tag_stack.pop();
  _s.tag_stack.push("[root]");
  _prevs = _s;
}

// _____________________________________________________________________________
inline size_t file::level() const { return _s.tag_stack.size() - _s.hanging; }

// _____________________________________________________________________________
inline parser_state file::state() { return _prevs; }

// _____________________________________________________________________________
inline void file::set_state(const parser_state& s) {
  _s = s;
  _prevs = s;

  if (_gzip) {
#ifndef PFXML_NO_ZLIB
    gzseek(_gzfile, _s.off, SEEK_SET);
#endif
  } else if (_bzip) {
#ifndef PFXML_NO_BZLIB
    int err;

    // simulate seek
    if (_bzfile) {
      int err;
      BZ2_bzReadClose(&err, _bzfile);
      _bzfile = 0;
    }
    if (_bzfhandle) {
      fclose(_bzfhandle);
      _bzfhandle = 0;
    }

    _bzfhandle = fopen(_path.c_str(), "r");
    if (!_bzfhandle)
      throw parse_exc(std::string("could not open file A"), _path, 0, 0, 0);

    _bzfile = BZ2_bzReadOpen(&err, _bzfhandle, 0, 0, NULL, 0);

    if (!_bzfile || err != BZ_OK) {
      throw parse_exc(std::string("could not read bzip file"), _path, 0, 0, 0);
    }

    int64_t readSoFar = 0;

    while (err == BZ_OK) {
      int readb;
      if (readSoFar + int64_t(BUFFER_S) > _s.off) {
        readb = BZ2_bzRead(&err, _bzfile, _buf[_which], _s.off - readSoFar);
      } else {
        readb = BZ2_bzRead(&err, _bzfile, _buf[_which], BUFFER_S);
      }
      if (readb == 0) break;
      readSoFar += readb;
    }
    assert(readSoFar == _s.off);
#endif
  } else {
    lseek(_file, _s.off, SEEK_SET);
  }
  _tot_read_bef = _s.off;

  if (_gzip) {
#ifndef PFXML_NO_ZLIB
    _last_bytes = gzread(_gzfile, _buf[_which], BUFFER_S);
#endif
  } else if (_bzip) {
#ifndef PFXML_NO_BZLIB
    int err;
    _last_bytes = BZ2_bzRead(&err, _bzfile, _buf[_which], BUFFER_S);
#endif
  } else {
    _last_bytes = read(_file, _buf[_which], BUFFER_S);
  }
  _last_new_data = _last_bytes;
  _c = _buf[_which];

  next();
}

// _____________________________________________________________________________
inline const tag& file::get() const { return _ret; }

// _____________________________________________________________________________
inline bool file::next() {
  if (!_s.tag_stack.size()) return false;
  // avoid too much stack copying
  if (_prevs.tag_stack.size() != _s.tag_stack.size() ||
      _prevs.tag_stack.top() != _s.tag_stack.top()) {
    _prevs.tag_stack = _s.tag_stack;
  }
  _prevs.s = _s.s;
  _prevs.hanging = _s.hanging;
  _prevs.off =
      _tot_read_bef + (_c - _buf[_which]) - (_last_bytes - _last_new_data);

  if (_s.hanging) _s.hanging--;
  _ret.name = 0;
  _ret.text = empty_str;
  _ret.attrs.clear();
  void* i;
  while (_last_bytes) {
    for (; _c - _buf[_which] < _last_bytes; ++_c) {
      char c = *_c;
      switch (_s.s) {
        case NONE:
          if (std::isspace(c))
            continue;
          else if (c == '<') {
            _s.s = IN_TAG_TENTATIVE;
            continue;
          }
          _s.s = IN_TEXT;
          _ret.name = empty_str;
          _tmp = _c;
          continue;

        case IN_TEXT:
          if (_s.tag_stack.size() == 1) {
            throw parse_exc("No text allowed here.", _path, _c, _buf[_which],
                            _prevs.off);
          }
          i = memchr(_c, '<', _last_bytes - (_c - _buf[_which]));
          if (!i) {
            _c = _buf[_which] + _last_bytes;
            continue;
          }
          _c = (char*)i;
          *_c = 0;
          _ret.text = _tmp;
          _s.s = IN_TAG_TENTATIVE;
          _c++;
          return true;

        case IN_COMMENT_TENTATIVE:
          if (c == '-') {
            _s.s = IN_COMMENT_TENTATIVE2;
            continue;
          }
          throw parse_exc("Expected comment", _path, _c, _buf[_which],
                          _prevs.off);

        case IN_COMMENT_TENTATIVE2:
          if (c == '-') {
            _s.s = IN_COMMENT;
            continue;
          }
          throw parse_exc("Expected comment", _path, _c, _buf[_which],
                          _prevs.off);

        case IN_COMMENT_CL_TENTATIVE:
          if (c == '-') {
            _s.s = IN_COMMENT_CL_TENTATIVE2;
            continue;
          }
          _s.s = IN_COMMENT;
          continue;

        case IN_COMMENT_CL_TENTATIVE2:
          if (c == '>') {
            _s.s = NONE;
            continue;
          }
          _s.s = IN_COMMENT;
          // fall through, we are still in comment

        case IN_COMMENT:
          i = memchr(_c, '-', _last_bytes - (_c - _buf[_which]));
          if (!i) {
            _c = _buf[_which] + _last_bytes;
            continue;
          }
          _c = (char*)i;
          _s.s = IN_COMMENT_CL_TENTATIVE;
          continue;

        case IN_TAG_TENTATIVE:
          if (c == '/') {
            _s.s = IN_TAG_NAME_CLOSE;
            _tmp = _c + 1;
            continue;
          } else if (c == '?') {
            _s.s = IN_TAG_NAME_META;
            continue;
          } else if (c == '!') {
            _s.s = IN_COMMENT_TENTATIVE;
            continue;
          } else if (std::isalnum(c) || c == '-' || c == '_' || c == '.') {
            _s.s = IN_TAG_NAME;
            _ret.name = _c;
            continue;
          }

        case IN_TAG:
          if (std::isspace(c))
            continue;
          else if (std::isalnum(c) || c == '-' || c == '_' || c == '.') {
            _s.s = IN_ATTRKEY;
            _tmp = _c;
            continue;
          } else if (c == '/') {
            _s.s = AW_CLOSING;
            continue;
          } else if (c == '>') {
            _s.hanging++;
            _s.tag_stack.push(_ret.name);
            _s.s = WS_SKIP;
            continue;
          }
          throw parse_exc("Expected valid tag", _path, _c, _buf[_which],
                          _prevs.off);

        case IN_ATTRVAL_SQ:
          i = memchr(_c, '\'', _last_bytes - (_c - _buf[_which]));
          if (!i) {
            _c = _buf[_which] + _last_bytes;
            continue;
          }
          _c = (char*)i;
          _s.s = IN_TAG;
          *_c = 0;
          _ret.attrs.push_back({_tmp, _tmp2});
          continue;

        case IN_ATTRVAL_DQ:
          i = memchr(_c, '"', _last_bytes - (_c - _buf[_which]));
          if (!i) {
            _c = _buf[_which] + _last_bytes;
            continue;
          }
          _c = (char*)i;
          _s.s = IN_TAG;
          *_c = 0;
          _ret.attrs.push_back({_tmp, _tmp2});
          continue;

        case AW_IN_ATTRVAL:
          if (std::isspace(c))
            continue;
          else if (c == '\'') {
            _s.s = IN_ATTRVAL_SQ;
            _tmp2 = _c + 1;
            continue;
          } else if (c == '"') {
            _s.s = IN_ATTRVAL_DQ;
            _tmp2 = _c + 1;
            continue;
          }
          throw parse_exc("Expected attribute value", _path, _c, _buf[_which],
                          _prevs.off);

        case IN_ATTRKEY:
          if (std::isspace(c)) {
            *_c = 0;
            _s.s = AFTER_ATTRKEY;
            continue;
          } else if (std::isalnum(c) || c == '-' || c == '_' || c == '.') {
            continue;
          } else if (c == '=') {
            *_c = 0;
            _s.s = AW_IN_ATTRVAL;
            continue;
          }

          throw parse_exc("Expected attribute key char or =", _path, _c,
                          _buf[_which], _prevs.off);

        case AFTER_ATTRKEY:
          if (std::isspace(c))
            continue;
          else if (c == '=') {
            _s.s = AW_IN_ATTRVAL;
            continue;
          }
          throw parse_exc(
              std::string("Expected attribute value for '") + _tmp + "'.",
              _path, _c, _buf[_which], _prevs.off);

        case IN_TAG_NAME:
          if (std::isspace(c)) {
            *_c = 0;
            _s.s = IN_TAG;
            continue;
          } else if (c == '>') {
            *_c = 0;
            _s.hanging++;
            _s.tag_stack.push(_ret.name);
            _s.s = WS_SKIP;
            continue;
          } else if (c == '/') {
            *_c = 0;
            _s.s = AW_CLOSING;
            continue;
          } else if (std::isalnum(c) || c == '-' || c == '_' || c == '.') {
            continue;
          }

        case IN_TAG_NAME_META:
          // TODO: read meta tags!
          if (c == '>') {
            _s.s = NONE;
            continue;
          }

          continue;

        case IN_TAG_NAME_CLOSE:
          if (std::isspace(c)) {
            *_c = 0;
            _s.s = IN_TAG_CLOSE;
            continue;
          } else if (std::isalnum(c) || c == '-' || c == '_' || c == '.') {
            continue;
          } else if (c == '>') {
            *_c = 0;
            if (_tmp != _s.tag_stack.top()) {
              throw parse_exc(std::string("Closing wrong tag '<") + _tmp +
                                  ">', expected close of '<" +
                                  _s.tag_stack.top() + ">'.",
                              _path, _c, _buf[_which], _prevs.off);
            }
            _s.tag_stack.pop();
            _s.s = NONE;
            continue;
          }

        case IN_TAG_CLOSE:
          if (std::isspace(c))
            continue;
          else if (c == '>') {
            if (_tmp != _s.tag_stack.top()) {
              throw parse_exc(std::string("Closing wrong tag '<") + _tmp +
                                  ">', expected close of '<" +
                                  _s.tag_stack.top() + ">'.",
                              _path, _c, _buf[_which], _prevs.off);
            }
            _s.tag_stack.pop();
            _s.s = NONE;
            continue;
          }
          throw parse_exc("Expected '>'", _path, _c, _buf[_which], _prevs.off);

        case AW_CLOSING:
          if (c == '>') {
            _s.s = WS_SKIP;
            continue;
          }

        case WS_SKIP:
          if (std::isspace(c)) continue;
          _s.s = NONE;
          return true;
      }
    }

    // buffer ended, read new stuff, but copy remaining if needed
    size_t off = 0;
    if (_s.s == IN_TAG_NAME) {  //|| IN_TAG_NAME_META) {
      off = _last_bytes - (_ret.name - _buf[_which]);
      memmove(_buf[!_which], _ret.name, off);
      _ret.name = _buf[!_which];
    } else if (_s.s == IN_TAG_NAME_CLOSE || _s.s == IN_ATTRKEY ||
               _s.s == IN_TEXT) {
      off = _last_bytes - (_tmp - _buf[_which]);
      memmove(_buf[!_which], _tmp, off);
      _tmp = _buf[!_which];
    } else if (_s.s == IN_ATTRVAL_SQ || _s.s == IN_ATTRVAL_DQ) {
      off = _last_bytes - (_tmp2 - _buf[_which]);
      memmove(_buf[!_which], _tmp2, off);
      _tmp2 = _buf[!_which];
    }

    assert(off <= BUFFER_S);

    size_t readb = 0;
    if (_gzip) {
#ifndef PFXML_NO_ZLIB
      readb = gzread(_gzfile, _buf[!_which] + off, BUFFER_S - off);
#endif
    } else if (_bzip) {
#ifndef PFXML_NO_BZLIB
      int err;
      readb = BZ2_bzRead(&err, _bzfile, _buf[!_which] + off, BUFFER_S - off);
#endif
    } else {
      readb = read(_file, _buf[!_which] + off, BUFFER_S - off);
    }
    if (!readb) break;
    _tot_read_bef += _last_new_data;
    _which = !_which;
    _last_new_data = readb;
    _last_bytes = _last_new_data + off;
    _c = _buf[_which] + off;
  }

  if (_s.tag_stack.size()) {
    if (_s.tag_stack.top() != "[root]") {
      throw parse_exc("XML tree not complete", _path, _c, _buf[_which],
                      _prevs.off);
    }
    _s.tag_stack.pop();
  }
  _s.s = NONE;
  _ret.name = "[root]";
  return false;
}

// _____________________________________________________________________________
inline std::string file::decode(const std::string& str) {
  return decode(str.c_str());
}

// _____________________________________________________________________________
inline std::string file::decode(const char* str) {
  const char* c = strchr(str, '&');
  if (!c) return str;

  char* dec_ret = new char[strlen(str) + 1];
  const char* last = str;
  char* dst_pt = dec_ret;

  for (; c != 0; c = strchr(c + 1, '&')) {
    memcpy(dst_pt, last, c - last);
    dst_pt += c - last;
    last = c;

    if (*(c + 1) == '#') {
      uint64_t cp = -1;
      char* tail;
      errno = 0;
      if (*(c + 2) == 'x' || *(c + 2) == 'X')
        cp = strtoul(c + 3, &tail, 16);
      else
        cp = strtoul(c + 2, &tail, 10);

      if (*tail == ';' && cp <= 0x1FFFFF && !errno) {
        dst_pt += utf8(cp, dst_pt);
        last = tail + 1;
      }
    } else {
      const char* e = strchr(c, ';');
      if (e) {
        char* ent = new char[e - 1 - c + 1];
        memcpy(ent, c + 1, e - 1 - c);
        ent[e - 1 - c] = 0;
        const auto it = ENTITIES.find(ent);
        if (it != ENTITIES.end()) {
          const char* utf8 = it->second;
          memcpy(dst_pt, utf8, strlen(utf8));
          dst_pt += strlen(utf8);
          last += strlen(ent) + 2;
        }
        delete[] ent;
      }
    }
  }

  strcpy(dst_pt, last);
  std::string ret(dec_ret);
  delete[] dec_ret;
  return ret;
}

// _____________________________________________________________________________
inline size_t file::utf8(size_t cp, char* out) {
  if (cp <= 0x7F) {
    out[0] = cp & 0x7F;
    return 1;
  } else if (cp <= 0x7FF) {
    out[0] = 0xC0 | (cp >> 6);
    out[1] = 0x80 | (cp & 0x3F);
    return 2;
  } else if (cp <= 0xFFFF) {
    out[0] = 0xE0 | (cp >> 12);
    out[1] = 0x80 | ((cp >> 6) & 0x3F);
    out[2] = 0x80 | (cp & 0x3F);
    return 3;
  } else if (cp <= 0x1FFFFF) {
    out[0] = 0xF0 | (cp >> 18);
    out[1] = 0x80 | ((cp >> 12) & 0x3F);
    out[2] = 0x80 | ((cp >> 6) & 0x3F);
    out[3] = 0x80 | (cp & 0x3F);
    return 4;
  }

  return 0;
}
}  // namespace pfxml

#endif  // PFXML_H_
