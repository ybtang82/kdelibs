/*
    Copyright (C) 2000 Dawit Alemayehu <adawit@kde.org>
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   This KMD5 class is based on a C++ implementation of
   "RSA Data Security, Inc. MD5 Message-Digest Algorithm" by   
   Mordechai T. Abzug,	Copyright (c) 1995.  The implementation
   passes the test-suite as defined by RFC 1321.
   
   RFC 1321 "MD5 Message-Digest Algorithm"
   Copyright (C) 1991-1992, RSA Data Security, Inc.
   Created 1991. All rights reserved.
      
   The encode/decode utilities in KCodecs were adapted from
   Ronald Tschal�r Copyright (C) 1996-1999 HTTPClient java
   pacakge.
*/

#ifndef _KMDBASE_H
#define _KMDBASE_H

#include <stdio.h>
#include <qglobal.h>

typedef char HASH[16];
typedef char HASHHEX[33];

#define KBase64 KCodecs

class QString;
class QCString;

class KCodecs
{
public:

   /**
    * This method encodes the given string using the unix uuencode
    * encoding.
    *
    * The output is split into lines starting with the number of
    * encoded octets in the line and ending with a newline.  No
    * line is longer than 45 octets (60 characters), excluding the
    * line terminator.
    *
    * <em>Note:</em> @p data should be just the actual data ; any
    * 'begin' and 'end' lines such as those generated by *nix
    * @p uuencode utility must not be included.
    *
    * @param  data the data
    * @return the uuencoded <var>data</var>
    */
    static QString uuencode( const QString& data );

   /**
    * This method decodes the given uuencoded string.
    *
    * <em>Note:</em> @p data should be just the actual data.
    * Any 'begin' and 'end' lines such as those generated by
    * *nix @p uuencode utility must not be included.
    *
    * @param  data the uuencode-encoded data.
    * @return the decoded @p data.
    */
    static QString uudecode( const QString& data );

   /**
    * This method encodes the given string using the base64-encoding
    * specified in RFC-2045 (Section 6.8). It's used for example in
    * the "Basic" authorization scheme (RFC 2617).
    *
    * @param data  the string.
    * @return the base64-encoded string.
    */
    static QString base64Encode( const QString& data );

   /**
    * This method decodes the given string using the
    * base64-encoding as specified in RFC-2045 (Section 6.8).
    *
    * @param data  the base64-encoded string.
    * @return the decoded string.
    */
    static QString base64Decode( const QString& data );

   /**
    * @deprecated. Use base64Encode instead.
    */
    static QString encodeString( const QString& data ) { return base64Encode(data); }

   /**
    * @deprecated. Use base64Decode instead.
    */
    static QString decodeString( const QString& data ) { return base64Decode(data); }

private:
    KCodecs();
};

/**
 *
 * This class provides an easy to use C++ implementation of the MD5
 * algorithm.  It provides an easy way to use the class much like
 * you would the plain C-implementations along with single shot
 * constructors that automatically perform the update and call
 * @ref finalize. 
 *
 * The default constructor is intended to provide much the same 
 * functionality as the C-implementation while the other three 
 * constructors are used to perform quick digest calculations whenever
 * the message to be does not require subsequent updating.  Additionally,
 * you can obtain either the result in a 16-byte binary format or
 * 33-byte hexidecimal by invoking @ref rawDigest and @ref hexDigest
 * respectivelly.  NOTE the extra byte in the latter format is is a
 * NULL character to terminate the string.
 *
 * @sect Example:
 *
 * The simplest way of using this class through one of the
 * accessor methods:
 *
 * <PRE>
 *  KMD5 context( QCString("ONE") );
 *  printf ( "Raw Digest output: %s", static_cast<char*>(context.rawDigest()) );
 *  printf ( "Hex Digest output: %s", context.hexDigest() );
 * </PRE>
 *
 * You can then invoke @ref reset() to re-use the same class to perform
 * another digest.
 *
 * <PRE>
 *  context.reset();
 *  context.update( QCString("TWO") );
 *  context.update( QCString("THREE") );
 *  printf ( "Raw Digest output: %s", static_cast<char*>(context.rawDigest()) );
 *  printf ( "Hex Digest output: %s", context.hexDigest() );
 * </PRE>
 *
 * Note that once you invoke @ref reset(), the previouly calculated
 * digest value will be lost.  Thus, be sure you copy the result if
 * you need to use it again.  Also you cannot invoke update() if you
 * use one of the convienence constructors unless you first invoke
 * reset().  Hence, if you need to update the message to be digested,
 * it is better to use the default constructor to avoid this subtle
 * error.
 *
 * The conventional (C-implemetation like) method of using this class:
 *
 * <PRE>
 *  KMD5 context;
 *  context.update(QCString("ONE"));
 *  context.update(QCString("TWO"));
 *  context.update(QCString("THREE"));
 *  context.finalize();
 *  printf ( "Raw Digest output: %s", static_cast<char*>(context.rawDigest()) );
 *  printf ( "Hex Digest output: %s", context.hexDigest() );
 * </PRE>
 *
 * @short An adapted C++ implementation of RSA Data Securities MD5 algorithm.
 * @author Dawit Alemayheu <adawit@kde.org>
 */
class KMD5
{

public:

  /**
   * HEX    hexidecimal representation of the message digest
   * BIN    binary representation of the message digest
   */
  enum DigestType { BIN, HEX };

  /**
   * ERR_NONE                   no error occured. [default]
   * ERR_ALREADY_FINALIZED      @ref finalize() has already been invoked.
   * ERR_NOT_YET_FINALIZED      @ref hexDigest() or @ref rawDigest() invoked before @ref finalize().
   * ERR_CANNOT_READ_FILE       indicates a problem while trying to read the given file.
   * ERR_CANNOT_CLOSE_FILE      indicates a problem while trying to close the given file.
   */
  enum ErrorType { ERR_NONE, ERR_ALREADY_FINALIZED, ERR_NOT_YET_FINALIZED,
                   ERR_CANNOT_READ_FILE, ERR_CANNOT_CLOSE_FILE };

  /**
   * Default constructor that only performs initialization.
   * Unlike the other constructors
   */
  KMD5();

  /**
   * Constructor that initializes, computes, and finalizes
   * the message digest for the given file.
   *
   * NOTE: This is a convience constructor.  As such it does
   * not allow the update of the message after it has been
   * invoked.  If you need to update the message after creating
   * the constructor,
   */
  KMD5(FILE *file);

  /**
   * Constructor that initializes, computes, and finalizes
   * the message digest for the given string.
   */
  KMD5(Q_UINT8 * input);       // digest string, finalize

  /**
   * Same as above except it takes a QCString as input.
   */
  KMD5(const QCString& input);

  /**
   * Same as above except it takes a QString as input.
   */
  KMD5(const QString& input);

  /**
   * Updates the message to be digested.
   *
   * @param input  message to be added to digest (unsigned char*)
   * @param len    the length of the given message. 
   */
  void update (Q_UINT8 * input, int len = -1 );

  /**
   * Same as above except except it takes a QCString as the argument.
   */
  void update ( const QCString& input );

  /**
   * Same as above except it takes a QString as the argument.
   */
  void update ( const QString& input );

  /**
   * Same as above except it accepts a pointer to FILE.
   *
   * NOTE that the file must have been already opened.  If you
   * want the file to be automatically closed, set @p closeFile
   * to TRUE.
   *
   * @param file       a pointer to FILE as returned by calls like f{d,re}open
   * @param closeFile  if true closes the file using fclose.  
   */
  void update (FILE *file, bool closeFile = false );

  /**
   * Finalizes the message digest calculation.
   *
   * If you used the default constructor, you must invoke this function
   * before you can obtain the calculated digest value.
   */
  void finalize();

  /**
   * Compares the message digest supplied messaged digest @p msg_digest
   * with the one calculated for the input QCString @p input.   
   *
   * NOTE: Calling this function will reset any previously calcualted
   * digests.  If you want to verify your token with the current digest
   * value, use @ref verify( const char*, DigestType ) instead.
   *
   * @param input       the message to be added to the digest value
   * @param msg_digest  the digest to compare the result against
   * @param type        the format of the result for comparison (binary or hexidecimal).
   *
   * @return true if the digests match, otherwise false.
   */
  bool verify( const QCString& input, const char * msg_digest,
               DigestType type = HEX );

  /**
   * Same as above except the input is a QString instead.
   *
   * NOTE: Calling this function will reset any previously calcualted
   * digests.  If you want to verify your token with the current digest
   * value, use @ref verify( const char*, DigestType ) instead.
   */
  bool verify( const QString& input, const char * msg_digest,
               DigestType type = HEX );

  /**
   * Same as above except the input is a pointer for a FILE instead.
   *
   * NOTE: Calling this function will reset any previously calcualted
   * digests.  If you want to verify your token with the current digest
   * value, use @ref verify( const char*, DigestType ) instead.
   */
  bool verify( FILE* f, const char * msg_digest, DigestType type = HEX );
  
  /**
   * Compares the given string with with the current message digest.
   *
   * NOTE: Unlike the other verify functions this one does not reset
   * the current message digest if one is already present.  It is meant
   * to allow you to compare against the existing message digest.  Also
   * note that this function will return false if a digest has yet to be
   * calculated.
   *
   * @param msg_digest  the digest to compare the result against
   * @param type        the format of the result for comparison (binary or hexidecimal).
   *
   * @return true if the digests match, otherwise false.
   */
  bool verify( const char* msg_digest, DigestType type = HEX );

  /**
   *  Re-initializes internal paramters.
   *
   * Note that calling this function will reset all internal variables
   * and hence any calculated digest.  Invoke this function only when
   * you have to re-use the same object to perform another message digest
   * calculation.
   */
  void reset();

  /**
   * Returns the raw 16-byte binary value of the message digest.
   *
   * NOTE: you are responsible for making a copy of this string.
   */
  Q_UINT8* rawDigest ();  // digest as a 16-byte binary array

  /**
   * Fills the given array with the binary representation of the
   * message digest.
   *
   * Use this method if you do not want to worry about making
   * copy of the digest once you obtain it.
   *
   * @param bin an array of 16 characters ( char[16] )
   * @return true if the raw digest is ready, otherwise false.
   */
  void rawDigest( HASH bin );

  /**
   * Returns a the value of the calculated message digest.
   * This is a 32 byte hexidecimal value terminated by a NULL
   * character.
   *
   * NOTE: you are responsible for making a copy of this string.
   */
  char * hexDigest ();  // digest as a 33-byte ascii-hex string

  /**
   * Fills the given array with the hexcidecimal representation of
   * the message digest.
   *
   * Use this method if you do not want to worry about making
   * copy of the digest once you obtain it. Also note that this
   * method will append a terminating NULL charater.
   *
   * @param bin an array of 33 characters ( char[33] )
   * @return true if the digest is ready, otherwise false.
   */
  void hexDigest( HASHHEX hex );

  /**
   * Indicates whether the message digest calculation failed
   * or succeeded.  Use @ref error to determine the error type.
   *
   * @return true if 
   */
  bool hasErrored() const { return (m_error != ERR_NONE); }

  /**
   * Returns the type error that occurred. See @ref ErrorType
   * for descriptions.
   */
  int error() const { return m_error; }

protected:

  /**
   *  Initializer called by all constructors
   */
  void init();

  /**
   *  Performs the real update work.  Note
   *  that length is implied to be 64.
   */
  void transform( Q_UINT8 * buffer );

  /**
   * Returns true if the current message digest matches @p msg_digest.
   */
  bool isDigestMatch( const char * msg_digest, DigestType type );

private:

  Q_UINT32 m_state[4];
  Q_UINT32 m_count[2];        // number of *bits*, mod 2^64
  Q_UINT8 m_buffer[64];       // input buffer
  Q_UINT8 m_digest[16];
  ErrorType m_error;
  bool m_finalized;

  struct KMD5Private;
  KMD5Private* d;
};
#endif
