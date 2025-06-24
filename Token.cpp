#include "Token.hpp"
#include <memory>
#include <boost/tokenizer.hpp>

typedef boost::char_separator<wchar_t> BoostSeparator;
typedef boost::tokenizer<BoostSeparator, std::wstring::const_iterator, std::wstring> BoostTokenizer;

namespace Memphis
{

Token::Token(Token* parent,
	const std::wstring& text,
	const std::wstring& separators,
	bool discard) :
	mParent(parent),
	mText(text),
	mSeparators(separators),
	mDiscard(discard)
{

}

Token::~Token()
{

}


void Token::Split()
{
    if (mSeparators.empty())
        return;

    BoostSeparator separ(mSeparators.c_str());
    BoostTokenizer tokenizer(mText, separ);

    mSubTokens.clear();

    //  does it have only one token?
    BoostTokenizer::iterator iter = tokenizer.begin();
    for (; iter != tokenizer.end(); ) {
        bool equals = (mText == std::wstring(*iter));

        ++iter;
        if (iter == tokenizer.end() && equals) {
            return; //  no subtokens, bail out
        }
        else 
            break;  //  has more than one token, add them
    }

    iter = tokenizer.begin();
    int count = 0;
    for (; iter != tokenizer.end(); ++iter) {
        //	create sub-tokens
        SharedPtrToken sub (new Token(this, *iter, L"", mDiscard));
        mSubTokens.push_back (sub);

        ++count;
    }
}

}
