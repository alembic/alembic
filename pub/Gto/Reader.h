//
//  Copyright (c) 2009, Tweak Software
//  All rights reserved.
// 
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//     * Redistributions of source code must retain the above
//       copyright notice, this list of conditions and the following
//       disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials
//       provided with the distribution.
//
//     * Neither the name of the Tweak Software nor the names of its
//       contributors may be used to endorse or promote products
//       derived from this software without specific prior written
//       permission.
// 
//  THIS SOFTWARE IS PROVIDED BY Tweak Software ''AS IS'' AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL Tweak Software BE LIABLE FOR
//  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
//  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
//  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
//
#ifndef __Gto__Reader__h__
#define __Gto__Reader__h__
#include <Gto/Header.h>
#include <Gto/Utilities.h>
#include <iostream>
#include <map>
#include <string>
#include <string>
#include <vector>
#include <list>

#if defined(None) && defined(X_H)
// WARNING: You included X.h which defines None
// so we're going to undef it here
#undef None
#endif

class GTOFlexLexer;

namespace Gto {

//
//  class Reader
//
//  Reads a GTO file as a byte stream or with random access. If compiled
//  with GTO_SUPPORT_ZIP defined, it can read gzipped gto files directly.
//

class Reader
{
public:
    //
    //  Types
    //

    struct ObjectInfo;
    struct ComponentInfo;
    struct PropertyInfo;
    friend class ::GTOFlexLexer;   // for ascii parser

    struct ObjectInfo : ObjectHeader
    {
        void*               objectData; // whatever was returned from object()

        int componentOffset() const { return coffset; }

    private:
        int                 coffset;
        bool                requested;
        friend class Reader;
    };

    struct ComponentInfo : ComponentHeader
    {
        void*               componentData; // return value of component()
        const ObjectInfo*   object;

        int propertyOffset() const { return poffset; }

    private:
        int                 poffset;
        bool                requested;
        friend class Reader;
    };

    struct PropertyInfo : PropertyHeader
    {
        void*                propertyData;
        unsigned int         offset;    // file offset

        const ComponentInfo* component;

    private:
        bool                 requested;
        size_t               index;
        friend class Reader;
    };

    typedef std::vector<ObjectInfo>    Objects;
    typedef std::vector<ComponentInfo> Components;
    typedef std::vector<PropertyInfo>  Properties;
    typedef std::vector<std::string>   StringTable;
    typedef std::vector<unsigned char> ByteArray;
    typedef std::map<std::string,int>  StringMap;
    typedef std::vector<unsigned int>  DataOffsets;


    //
    //  The open modes:
    //
    //  None: the file is read as a stream with default API
    //  behavior. The file may be text, binary, or compressed binary. 
    //
    //  BinaryOnly: only binary and compressed binary files will be
    //  accepted. 
    //
    //  TextOnly: only text files will be accepted.
    //
    //  HeaderOnly: only the header information will be read. This is
    //  equivalent to requesting none of the data, but it will short
    //  circuit the reader into finishing successfully without seeking
    //  through the file. 
    //
    //  RandomAccess: the file should be prepared for random access:
    //  no header functions or data will be called until you
    //  specifically ask for an object by name -- then they will
    //  called as if the file contained only that data. You can do
    //  this as many times as you want using the accessObject()
    //  function. RandomAccess implies BinaryOnly.
    //
    //

    enum ReadMode
    {
        None             = 0,
        HeaderOnly       = 1 << 0,
        RandomAccess     = 1 << 1,
        BinaryOnly       = 1 << 2,
        TextOnly         = 1 << 3,
    };

    explicit Reader(unsigned int mode = None);
    virtual ~Reader();

    //
    //  Open takes the path to the .gto file. If the GTO_SUPPORT_ZIP
    //  is enabled then open will automatically attempt to find a
    //  gziped version of the file if the file does not exist.
    //
    //  If the mode is RandomAccess, then open will return having only read
    //  the header information.
    //
    //  The stream open function can take additional ReadMode enum
    //  to modify the input type. The ormode is |'d with the open mode.
    //

    virtual bool        open(void const *pData, size_t dataSize, const char *name);
    virtual bool        open(const char *filename);
    virtual bool        open(std::istream&, const char *name, 
                             unsigned int ormode = 0);
    void                close();

    //
    //  If it failed. why() will return a description.
    //
    void                fail( std::string why = "" );

    const std::string&  why() const { return m_why; }

    const std::string&  stringFromId(unsigned int i);
    const StringTable&  stringTable() { return m_strings; }

    bool                isSwapped() const { return m_swapped; }
    bool                hasIndex() const { return !m_dataOffsets.empty(); }
    unsigned int        readMode() const { return m_mode; }

    const std::string&  infileName() const { return m_inName; }

    std::istream*       in() const { return m_in; }
    int                 linenum() const { return m_linenum; }
    int                 charnum() const { return m_charnum; }

    Header&             fileHeader() { return m_header; }

    //
    //  This function is called right after the file header is read. 
    //

    virtual void        header(const Header&);

    //
    //  This function is called after all file, object, component, and
    //  property structures have been read. For binary files, this is
    //  just before the data is read. For text files, this is after the
    //  entire file has been read.
    //

    virtual void        descriptionComplete();

    //
    //  RandomAccess functions. When the file is openned as RandomAccess,
    //  you can get ahold of an ObjectInfo and pass it to this function to
    //  have the reader seek directly.
    //
    //  accessObject() requires that the reference passed in be from the
    //  reader's object cache, not a copy.
    //
    //  The components() and properties() functions are there to make it
    //  easy to take parts for later retrieval with accessObject()
    //

    Objects&            objects() { return m_objects; }
    bool                accessObject(ObjectInfo&);

    Components&         components() { return m_components; }
    bool                accessComponent(ComponentInfo&);

    Properties&         properties() { return m_properties; }
    bool                accessProperty(PropertyInfo&);

    //
    //  These are used to declare a component or property. The
    //  functions are called expecting the return value to be non-zero
    //  (or your own pointer) if the reader should try and read the
    //  data associated with the component or property -- the default
    //  is non-zero.
    //

    struct Request
    {
        Request()
            : m_want(true), m_data(0) {}
        Request(bool want, void* data = 0)
            : m_want(want), m_data(data) {}

        bool  want() const { return m_want; }
        void* data() const { return m_data; }

    private:
        bool        m_want;
        void*       m_data;
        friend class Reader;
    };

    //
    //  Version 3 API
    //

    virtual Request     object(const std::string& name,
                               const std::string& protocol,
                               unsigned int protocolVersion,
                               const ObjectInfo &header);


    virtual Request     component(const std::string& name,
                                  const std::string& interp,
                                  const ComponentInfo &header);


    virtual Request     property(const std::string& name,
                                 const std::string& interp,
                                 const PropertyInfo &header);

    //
    //  Version 2 API still works, if the version 3 API (above) is not
    //  overriden. New applications should not use these functions.
    //

    virtual Request     component(const std::string& name,
                                  const ComponentInfo &header);

    virtual Request     property(const std::string& name,
                                 const PropertyInfo &header);

    //
    //  Data should return a pointer to a region of memory large
    //  enough to hold the data that will be read for
    //  PropertyInfo. The bytes parameter tells you how much memory
    //  (in bytes) will be required
    //

    virtual void*       data(const PropertyInfo&, size_t bytes);

    //
    //  dataRead() (read "data red") is called after the data is
    //  succesfully read (so after the data() function is called)
    //

    virtual void        dataRead(const PropertyInfo&);

    //------------------------------------------------------------
    //
    //  Text file parser
    //

    //
    //  Override these to redirect error messages. If you throw
    //  you will stop the parsing
    //

    virtual void        parseError(const char* msg);
    virtual void        parseWarning(const char* msg);


    //
    //  Used by the text file parser
    //

    void                beginHeader(uint32 version);
    void                beginObject(unsigned int name,
                                    unsigned int protocol,
                                    unsigned int pversion=1);

    void                beginComponent(unsigned int name,
                                       unsigned int interp);

    void                beginProperty(unsigned int name,
                                      unsigned int interp,
                                      unsigned int width,
                                      unsigned int size,
                                      DataType type);

    void                endProperty();
    void                endFile();

    void                addObject(const ObjectInfo&);
    void                addComponent(const ComponentInfo&);

    const TypeSpec&     currentType() const { return m_currentType; }

    template <typename T>
    void                addToPropertyBuffer(T);

    size_t              numAtomicValuesInBuffer() const;
    size_t              numElementsInBuffer() const;
    void                fillToSize(size_t);

    int                 internString(const std::string&);

protected:
    //
    //  This function is responsible for calling the data functions
    //  above. If you need to do some unpacking at the stream level
    //  you can override this function. The function should return
    //  true on success, false otherwise.
    //

    virtual bool        readProperty(PropertyInfo&);

private:
    bool                readBinaryGTO();
    bool                readTextGTO();
    void                readMagicNumber();
    void                readHeader();
    void                readStringTable();
    void                readObjects();
    void                readComponents();
    void                readProperties();
    void                readIndexTable();

    void                read(char *, size_t);
    void                get(char &);
    bool                notEOF();
    void                seekForward(size_t);
    int                 tell();
    void                seekTo(const PropertyInfo &p);

private:
    Header              m_header;
    Objects             m_objects;
    Components          m_components;
    Properties          m_properties;
    StringTable         m_strings;
    StringMap           m_stringMap;
    std::istream*       m_in;
    char*               m_inRAM;
    size_t              m_inRAMSize;
    size_t              m_inRAMCurrentPos;
    void*               m_gzfile;
    int                 m_gzrval;
    std::string         m_inName;
    bool                m_needsClosing;
    bool                m_error;
    std::string         m_why;
    bool                m_swapped;
    unsigned int        m_mode;
    int                 m_linenum;
    int                 m_charnum;
    ByteArray           m_buffer;
    TypeSpec            m_currentType;
    size_t              m_currentReadOffset;
    DataOffsets         m_dataOffsets;
};

template <typename T>
void
Reader::addToPropertyBuffer(T val)
{
    size_t i = m_buffer.size();
    m_buffer.insert(m_buffer.end(), sizeof(T), 0);
    T* p = reinterpret_cast<T*>(&m_buffer[i]);
    *p = val;
}

} // Gto

#endif // __Gto__Reader__h__
