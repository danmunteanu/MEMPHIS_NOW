/*
 * This file is part of MemphisNow.
 *
 * MemphisNow is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MemphisNow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MemphisNow.  If not, see <https://www.gnu.org/licenses/>.
 */

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

    //  setup boost tokenizer
    BoostSeparator separ(mSeparators.c_str());
    BoostTokenizer tokenizer(mText, separ);

    //  cleanup previous token
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

    //  create sub-tokens
    iter = tokenizer.begin();
    int count = 0;
    for (; iter != tokenizer.end(); ++iter) {
        SharedPtrToken subtoken (new Token(this, *iter, L"", mDiscard));
        mSubTokens.push_back (subtoken);

        ++count;
    }
}

bool Token::IsSubtoken(const Token* token) const
{
    SharedTokensContainer::const_iterator iter = mSubTokens.cbegin();
    for (; iter != mSubTokens.end(); ++iter)
    {
        if (iter->get() == token)
        {
            return true;
        }

        if ((*iter)->CountSubtokens() > 0)
        {
            //  TBD: iterate subtokens
        }
    }
    return false;
}

}
