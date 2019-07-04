/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2011  Mark A Lindner

   This file is part of commonc++.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   ---------------------------------------------------------------------------
*/

#ifndef __ccxx_SHA1Digest_hxx
#define __ccxx_SHA1Digest_hxx

#include "xcore_define.h"
#include "xcore_noncopyable.h"

namespace xcore
{

/** A class for generating SHA-1 digests.
 *
 * @author Mark Lindner
 */

class SHA1Digest : XNoncopyable
{
public:

	/** Construct a new SHA1Digest. */
	SHA1Digest() throw();

	/** Destructor. */
	virtual ~SHA1Digest() throw();

	inline virtual size_t size() const throw()
	{ return (20); }

	virtual void update(const uint8* buf, size_t len);

	void final(uint8(&digest)[20]);

	// ·µ»Øsha1Ê®Áù½øÖÆ´óÐ´×ÖÄ¸×Ö·û´®
	string final();

	virtual void reset() throw();

private:

	void processBlock();
	void padBlock();
	inline uint32 rotateLeft(uint32 word, int bits);

	uint32 _h[5];
	uint64 _length;
	uint8 _block[64];
	int _offset;
};


void sha1(const void* src, uint32 size_, uint8(&digest)[20]);

string sha1(const void* src, uint32 size_);

void hmac_sha1(const void* src, uint32 size_, const void* key, uint32 key_size_, uint8(&digest)[20]);

string hmac_sha1(const void* src, uint32 size_, const void* key, uint32 key_size_);

// ¼ÆËãÎÄ¼þsha1,Ê§°Ü·µ»Ø¿Õ×Ö·û´®
string sha1_file(const string& filepath);

}; // namespace xcore

using namespace xcore;


#endif // __ccxx_SHA1Digest_hxx

/* end of header file */
