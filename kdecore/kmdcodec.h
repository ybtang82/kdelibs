/*
   Copyright (C) 2000-2001 Dawit Alemayehu <adawit@kde.org>

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
   Mordechai T. Abzug,	Copyright (c) 1995.  This implementation
   passes the test-suite as defined by RFC 1321.

   RFC 1321 "MD5 Message-Digest Algorithm" Copyright (C) 1991-1992.
   RSA Data Security, Inc. Created 1991. All rights reserved.

   The encode/decode utilities in KCodecs were adapted from
   Ronald Tschal�r Copyright (C) 1996-1999 HTTPClient java
   pacakge.
*/

#ifndef _KMDBASE_H
#define _KMDBASE_H

#include <stdio.h>
#include <qglobal.h>
#include <qstring.h>

typedef char HASH[16];
typedef char HASHHEX[33];

#define KBase64 KCodecs

/**
 * A wrapper class for the most commonly used encoding and
 * decoding algorithms.  Currently it provides encoding and
 * decoding facilities for the base64 and uuencode/uudecode
 * algorithms.
 *
 * @section Useage:
 *
 * <PRE>
 * QCString input = "Aladdin:open sesame";
 * QCString result = KCodecs::base64Encode( input );
 * printf ( "Result: %c", result.data() );
 * </PRE>
 *
 * Output should be
 * Result: QWxhZGRpbjpvcGVuIHNlc2FtZQ==
 *
 * <u>NOTE:</u> A unit test program is available under the tests
 * directory that exercises all the codecs provided here.  You
 * can compile the unit test program under @p kdelibs/kio/tests
 * by doing @p "make kmdcodectest".
 *
 * @short A collection of commonly used encoding/decoding algorithms.
 * @author Dawit Alemayheu <adawit@kde.org>
 */
class KCodecs
{
public:

 /**
  * Encodes the given data using the uuencode algorithm.
  *
  * The output is split into lines starting with the number of
  * encoded octets in the line and ending with a newline.  No
  * line is longer than 45 octets (60 characters), excluding the
  * line terminator.
  *
  * <u>NOTE:</u> @p data should be just the actual data.  Any
  * 'begin' and 'end' lines such as those generated by a *nix
  * @p uuencode utility must not be included.
  *
  * @param in   the data to be uuencoded
  * @return     a uuencoded data.
  */
  static QCString uuencode( const QByteArray& in );

 /**
  * Encodes the given data using the uuencode algorithm.
  *
  * Use this function if you want the result of the encoding
  * to be placed in another array and cut down the number of
  * copy opertation that have to be performed in the process.
  *
  * <u>NOTE:</u> the output array is first reset and then
  * resized to the appropriate size.
  *
  * @param in   the data to be uuencoded.
  * @param out  the container for the uudecoded data.
  */
  static void uuencode( const QByteArray& in, QByteArray& out );

 /**
  * Encodes the given string using the uuencode algorithm.
  *
  * @param str   the string to be uuencoded.
  * @return      a uuencoded string.
  */
  static QCString uuencode( const QCString& str );

 /**
  * Decodes the given data using the uuencode algorithm.
  *
  * <u>NOTE:</u> @p data should be just the actual data.
  * Any 'begin' and 'end' lines such as those generated by
  * *nix utilities must NOT be included.
  *
  * @param in   the data uuencoded data to be decoded.
  * @return     the decoded data.
  */
  static QCString uudecode( const QByteArray& in );

 /**
  * Decodes the given data using the uudecode algorithm.
  *
  * Use this function if you want the result of the decoding
  * to be placed in another array and cut down the number of
  * copy opertation that have to be performed in the process.
  *
  * <u>NOTE:</u> the output array is first reset and then
  * resized to the appropriate size.
  *
  * @param in   the uuencoded-data to be decoded.
  * @param out  the container for the uudecoded data.
  */
  static void uudecode( const QByteArray& in, QByteArray& out );

 /**
  * Decodes a uuencoded string.
  *
  * @param str   the string to be decoded.
  * @return      a uudecoded string.
  */
  static QCString uudecode( const QCString& str );

 /**
  * Encodes the given data using the base64 algorithm.
  *
  * @param in  the data to be encoded.
  * @return    a base64 encoded data.
  */
  static QCString base64Encode( const QByteArray& in );

 /**
  * Encodes the given data using the base64 algorithm.
  *
  * Use this function if you want the result of the encoding
  * to be placed in another array and cut down the number of
  * copy opertation that have to be performed in the process.
  *
  * <u>NOTE:</u> the output array is first reset and then
  * resized to the appropriate size.
  *
  * @param in   the data to be encoded using base64.
  * @param out  the container for the encoded data.
  */
  static void base64Encode( const QByteArray& in, QByteArray& out );

 /**
  * Encodes the given string using the base64 algorithm.
  *
  * @param str  the string to be encoded.
  * @return     the decoded string.
  */
  static QCString base64Encode( const QCString& str );

 /**
  * Decodes the given data that was encoded using the base64 algorithm.
  *
  * @param in   the base64-encoded data to be decoded.
  * @return     the decoded data.
  */
  static QCString base64Decode( const QByteArray& in );

 /**
  * Decodes the given data that was encoded with the base64 algorithm.
  *
  * Use this function if you want the result of the decoding
  * to be placed in another array and cut down the number of
  * copy opertation that have to be performed in the process.
  *
  * <u>NOTE:</u> the output array is first reset and then
  * resized to the appropriate size.
  *
  * @param in   the encoded data to be decoded.
  * @param out  the container for the decoded data.
  */
  static void base64Decode( const QByteArray& in, QByteArray& out );

 /**
  * Decodes a string encoded with the base64 algorithm.
  *
  * @param str  the base64-encoded string.
  * @return     the decoded string.
  */
  static QCString base64Decode( const QCString& str );


  /**
   * @deprecated.  Use @ref QCString base64Encode(const QCString&) instead!
   *
   * <u>IMPORTANT:</u> This function has depricated and will be
   * removed in future release.  This is done to avoid loss of data
   * from misuse of the function since it first converts the given
   * data into Latin-1.  Additionally this conversion is very slow!
   */
  static QString base64Encode( const QString& str ){
    return base64Encode(QCString(str.latin1())); }

  /**
   * @deprecated.  Use @ref QCString base64Decode(const QCString&) instead!
   *
   * <u>IMPORTANT:</u> This function has depricated and will be
   * removed in future release.  This is done to avoid loss of data
   * from misuse of the function since it first converts the given
   * data into Latin-1.  Additionally this conversion is very slow!
   */
  static QString base64Decode( const QString& str ) {
    return base64Decode(QCString(str.latin1())); }

  /**
   * @deprecated.  Use @ref QCString uuencode(const QCString&) instead!
   *
   * <u>IMPORTANT:</u> This function has depricated and will be
   * removed in future release.  This is done to avoid loss of data
   * from misuse of the function since it first converts the given
   * data into Latin-1.  Additionally this conversion is very slow!
   */
  static QString uuencode( const QString& str )  {
    return uuencode(QCString(str.latin1())); }

  /**
   * @deprecated.  Use @ref QCString uudecode(const QCString&) instead!
   *
   * <u>IMPORTANT:</u> This function has depricated and will be
   * removed in future release.  This is done to avoid loss of data
   * from misuse of the function since it first converts the given
   * data into Latin-1.  Additionally this conversion is very slow!
   */
  static QString uudecode( const QString& str )  {
    return uudecode(QCString(str.latin1())); }

  /**
   * @deprecated. See @ref base64Encode(const QString&) instead.
   */
  static QString encodeString( const QString& data ) {
    return base64Encode(data); }

  /**
   * @deprecated. See @ref base64Decode(const QString&) instead.
   */
  static QString decodeString( const QString& data ) {
    return base64Decode(data); }

private:
  KCodecs();

private:
 static char UUEncMap[64];
 static char UUDecMap[128];
 static char Base64EncMap[64];
 static char Base64DecMap[128];

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
 * respectivelly.  The extra byte in the latter format is is a NULL
 * character to terminate the string!
 *
 * @sect Useage:
 *
 * The simplest way of using this class through one of the
 * accessor methods:
 *
 * <PRE>
 *  HASH rawResult;
 *  HASHHEX hexResult;
 *  QByteArray array;
 *  QCString test1;
 *
 *  test1 = "This is a simple test.";
 *  KMD5 context( test1 );
 *  context.rawDigest( rawDigest );
 *  context.hexDigest( hexDigest );
 *  printf ( "Raw Digest output: %s", rawResult );
 *  printf ( "Hex Digest output: %s", hexResult );
 * </PRE>
 *
 * You can then invoke @ref reset() to re-use the same class to perform
 * another digest.  This cuts down on the unnecessary overhead that would
 * be generated from creating multiple KMD5 objects.
 *
 * <PRE>
 *  context.reset();
 *  context.update( QCString("TWO") );
 *  context.update( QCString("THREE") );
 *  printf ( "Raw Digest output: %s", static_cast<char*>(context.rawDigest()) );
 *  printf ( "Hex Digest output: %s", context.hexDigest() );
 * </PRE>
 *
 * Once you invoke @ref reset(), the previouly calculated digest value
 * will be lost.  Thus, be sure you copy the result if you need to use
 * it again.  Also, if you use one of the convienence constructors you
 * must first invoke reset(), before calling any of the update functions.
 * Otherwise, the update call will fail and return the already finalized
 * error.  As such if you need to update the message to be digested, it is
 * hihgly recommended that you use the default constructor.
 *
 * Another example much along the lines of how the c-implementation of this
 * algorithm is usually used:
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
   * the message digest for the given string.
   *
   * NOTE: This is a convience constructor.  It is provided to
   * allow compatiability with the C implementation of this digest.
   */
  KMD5(Q_UINT8 * in);

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
   * Same as above except it accepts a QByteArray as its argument.
   */
  KMD5(const QByteArray& a );


  /**
   * Same as above except it accepts a QCString as its argument.
   */
  KMD5(const QCString& in);

  /**
   * @deprcated.  Use @ref KMD5(const QCString& in) instead!
   *
   * <u>IMPORTANT:</u> This constructor has been depricated and
   * will be removed in future release.  This is done to avoid
   * loss of data from misuse of the function since it first
   * converts the given data into Latin-1.  Additionally, this
   * conversion is very slow!
   */
  KMD5(const QString& in);

  /**
   * Updates the message to be digested.
   *
   * @param input  message to be added to the digest (QByteArray).
   */
  void update (const QByteArray& in );

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
   * Updates the message to be digested.
   *
   * @param input  message to be added to digest (unsigned char*)
   * @param len    the length of the given message.
   */
  void update (Q_UINT8 * in, int len = -1 );

  /**
   * Same as above except it accepts a QCString as its argument.
   */
  void update ( const QCString& in );

  /**
   * @deprecated.  Use @ref update( const QCString& ) instead!
   *
   * <u>IMPORTANT:</u> This function has depricated and will be
   * removed in future release.  This is done to avoid loss of data
   * from misuse of the function since it first converts the given
   * data into Latin-1.  Additionally this conversion is very slow!
   */
  void update ( const QString& in );

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
  bool verify( const QCString& in, const char * msg_digest,
               DigestType type = HEX );

  /**
   * @deprecated. Use the above function instead!
   *
   * <u>IMPORTANT:</u> This function has depricated and will be
   * removed in future release.  This is done to avoid loss of data
   * from misuse of the function since it first converts the given
   * data into Latin-1.  Additionally this conversion is very slow!
   */
  bool verify( const QString& in, const char * msg_digest,
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
   * Unlike the other verify functions this one does not reset the current
   * message digest if one is already present.  It is meant to allow you to
   * compare against the existing message digest.  Note that this function
   * will also return false if a digest has yet to be calculated.  Use the
   * error functions to determine which is the case.
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
  Q_UINT8* rawDigest ();

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
  char * hexDigest ();

  /**
   * Fills the given array with the hexcidecimal representation of
   * the message digest.
   *
   * Use this method if you do not want to worry about making
   * copy of the digest once you obtain it. Also note that this
   * method will append a terminating NULL charater to form a
   * properly terminated string.
   *
   * @param bin an array of 33 characters ( char[33] )
   * @return true if the digest is ready, otherwise false.
   */
  void hexDigest( HASHHEX hex );

  /**
   * Indicates whether the message digest calculation failed
   * or succeeded.  Use @ref error() to determine the error type.
   *
   * @return true if
   */
  bool hasErrored() const { return (m_error != ERR_NONE); }

  /**
   * Returns the type of error that occurred.
   * See @ref ErrorType for descriptions.
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
  void encode( Q_UINT8 *output, Q_UINT32 *in, Q_UINT32 len );
  void decode( Q_UINT32 *output, Q_UINT8 *in, Q_UINT32 len );
  Q_UINT32 rotate_left( Q_UINT32 x, Q_UINT32 n );
  Q_UINT32 F( Q_UINT32 x, Q_UINT32 y, Q_UINT32 z );
  Q_UINT32 G( Q_UINT32 x, Q_UINT32 y, Q_UINT32 z );
  Q_UINT32 H( Q_UINT32 x, Q_UINT32 y, Q_UINT32 z );
  Q_UINT32 I( Q_UINT32 x, Q_UINT32 y, Q_UINT32 z );
  void FF( Q_UINT32& a, Q_UINT32 b, Q_UINT32 c, Q_UINT32 d,
           Q_UINT32 x, Q_UINT32  s, Q_UINT32 ac );
  void GG( Q_UINT32& a, Q_UINT32 b, Q_UINT32 c, Q_UINT32 d,
           Q_UINT32 x, Q_UINT32 s, Q_UINT32 ac );
  void HH( Q_UINT32& a, Q_UINT32 b, Q_UINT32 c, Q_UINT32 d,
           Q_UINT32 x, Q_UINT32 s, Q_UINT32 ac );
  void II( Q_UINT32& a, Q_UINT32 b, Q_UINT32 c, Q_UINT32 d,
           Q_UINT32 x, Q_UINT32 s, Q_UINT32 ac );

private:
  Q_UINT32 m_state[4];
  Q_UINT32 m_count[2];
  Q_UINT8 m_buffer[64];
  Q_UINT8 m_digest[16];
  ErrorType m_error;
  bool m_finalized;

  struct KMD5Private;
  KMD5Private* d;
};
#endif