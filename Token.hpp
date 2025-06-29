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

#pragma once

#include <string>
#include <memory>
#include <list>

namespace Memphis
{

class Token;

typedef std::shared_ptr<Token> SharedPtrToken;
typedef std::list<SharedPtrToken> SharedTokensContainer;

enum EInsertDirection {
	Left,
	Right
};

class Token
{
public:
	explicit Token(
		Token* parent = NULL, 
		const std::wstring& text = L"",
		const std::wstring& separators = L"",
		bool discard = false);
	~Token();

	void SetParent(Token* parent) { mParent.reset(parent); }
	std::shared_ptr<Token> Parent() const { return mParent; }

	void SetText(const std::wstring& text) { mText = text; }
	std::wstring Text() const { return mText; }

	void SetSeparators(const std::wstring& separators) { mSeparators = separators; }
	std::wstring Separators() const { return mSeparators; }

	void SetDiscard(bool discard) { mDiscard = discard; }
	bool IsDiscard() const { return mDiscard; }

	//  access to subtokens
	SharedTokensContainer::const_iterator SubtokensConstBegin() const { return mSubTokens.begin(); }
	SharedTokensContainer::const_iterator SubtokensConstEnd() const { return mSubTokens.end(); }
	SharedTokensContainer::iterator SubtokensBegin() { return mSubTokens.begin(); }
	SharedTokensContainer::iterator SubtokensEnd() { return mSubTokens.end(); }
	bool IsSubTokensEmpty() const { return mSubTokens.empty(); }
	size_t CountSubtokens() const { return mSubTokens.size(); }
		
	void ClearSubtokens() { mSubTokens.clear(); }

	/*  checks if token is in the list of subtokens */
	bool IsSubtoken(const Token* token) const;

	/* finds last subtoken, regardless if it's disabled or not*/
	//const MPSToken* last_subtoken() const;

	//const MPSToken* find_first_leaf_subtoken(bool include_discarded) const;
	//const MPSToken* find_last_leaf_subtoken(bool include_discarded) const;

	/*  creates and inserts a new subtoken at the specified position
		*  if pos is after the maximum possible position, it will insert on the last position
		*  similarly, if pos is equal to KLastSubtokenPosition, it will insert on the last position
		*/
	//void insert_subtoken(const std::wstring& text, size_t pos);

	/*  shifts the specified subtoken to the desired position - left or right
		*  does nothing if it is null
		*  if sub_token is leftmost, does nothing
		*  if sub_token is rightmost, does nothing
		*/
	//void shift_subtoken(MPSToken* sub_token, EMPSDirection direction);

	//  splits this token into subtokens based on current separators
	void Split();

protected:
	//static void discard_token(MPSToken& token, bool discard);

private:
	std::shared_ptr<Token>	mParent;
	std::wstring			mText;
	std::wstring			mSeparators;
	bool					mDiscard;

	SharedTokensContainer	mSubTokens; //  shared pointers to subtokens, if needed
};

};
