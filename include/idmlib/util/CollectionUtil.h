/**
 * @file CollectionUtil.h
 * @author Zhongxia Li
 * @date Jun 1, 2011
 * @brief 
 */
#ifndef COLLECTION_UTIL_H_
#define COLLECTION_UTIL_H_

#include <idmlib/idm_types.h>
#include <idmlib/util/idm_analyzer.h>
#include <idmlib/util/FSUtil.hpp>
#include <idmlib/util/IdMgrFactory.h>

#include <la/LA.h>
#include <la/util/UStringUtil.h>

#include <ir/index_manager/utility/system.h>
#include <ir/index_manager/index/LAInput.h>
// prevent conflicting with ScdParser of sf1-revolution
#include <sf1common/ScdParser.h>
#include <sf1common/PropertyValue.h>

using namespace boost::filesystem;
using namespace izenelib::ir::idmanager;
using namespace izenelib::ir::indexmanager;
using namespace izenelib::util;
using namespace idmlib::util;


NS_IDMLIB_UTIL_BEGIN

struct CollectionPath
{
    CollectionPath(const std::string& basePath)
    : basePath_(basePath)
    {
        idmlib::util::FSUtil::normalizeFilePath(basePath_);

        scdPath_ = basePath_ + "/scd/index";
        dataPath_ = basePath_ + "/collection-data/default-collection-dir";
        dataIdPath_ = dataPath_ + "/id";
    }

    std::string basePath_;
    std::string scdPath_;
    std::string dataPath_;
    std::string dataIdPath_;
};


/**
 * Base
 */
class CollectionProcessor
{
public:
    typedef std::vector<std::pair<izenelib::util::UString, izenelib::util::UString> >::iterator doc_properties_iterator;

public:
    /**
     *
     * @param colBasePath base directory of collection
     * @param laResPath LA(CMA) resource path
     * @param maxDoc max documents to be processed, not limited if it's 0.
     * @param encoding encoding of SCD
     */
    CollectionProcessor(
        const std::string& colBasePath,
        const std::string& laResPath,
        const std::vector<std::string>& processProperties,
        size_t maxDoc = 0,
        bool removeStopwords = false,
        izenelib::util::UString::EncodingType encoding = izenelib::util::UString::UTF_8)
    : colPath_(colBasePath)
    , laResPath_(laResPath)
    , maxDoc_(maxDoc)
    , encoding_(encoding)
    , curDocId_(0)
    , content_("", encoding)
    , pTermIdList_(new TermIdList())
    {
        std::vector<std::string>::const_iterator iter;
        for (iter = processProperties.begin(); iter != processProperties.end(); iter++)
        {
            izenelib::util::UString propertyName(*iter, encoding);
            propertyName.toLowerString();
            processProperties_[propertyName] = true;
        }

    	s_pIdManager_ = idmlib::IdMgrFactory::getIdManagerESA();

        createAnalyzer(removeStopwords);
    }

    virtual ~CollectionProcessor(){}
public:
    bool processSCD();

protected:
    /**
     * Sub class can override this function to process other properties if needed.
     * @param pDoc SCD document
     */
    virtual void processDocument()
    {
        content_.clear();
        bool hasDocid = false;

        for (izenelib::SCDDoc::iterator proIter = curSCDDoc_->begin(); proIter != curSCDDoc_->end(); proIter ++)
        {
            std::string spropertyName = proIter->first;
            izenelib::util::UString propertyName(spropertyName, izenelib::util::UString::UTF_8);
            izenelib::util::UString propertyValue = propstr_to_ustr(proIter->second);
            propertyName.toLowerString();

            if ( propertyName == izenelib::util::UString("docid", encoding_) ) {
                s_pIdManager_->getDocIdByDocName(propertyValue, curDocId_, false);
                hasDocid = true;
            }

            /*if ( propertyName == izenelib::util::UString("title", encoding_) ) {
                // increase the weight of title?
                content_ += propertyValue;
                content_ += propertyValue;
            }
            else if ( propertyName == izenelib::util::UString("content", encoding_)) {
                content_ += propertyValue;
            }*/

            if ( isProcessProperty(propertyName) )
            {
                content_ += propertyValue;
            }
            else {
                continue;
            }
        }

        //cout <<"docid: "<<curDocId_<<endl;
        if ( !hasDocid )
        {
            std::cerr<<"SCDoc ERROR: no DOCID detected, skipped."<<std::endl;
            return;
        }

        processDocumentContent(); // xxx
    }


    virtual void processDocumentContent()
    {
        //cout << la::to_utf8(content_) << endl;

        pTermIdList_->clear();
        pIdmAnalyzer_->GetTermIdList(s_pIdManager_, content_, *pTermIdList_);

//        TermIdList::iterator iter;
//        for (iter = pTermIdList_->begin(); iter != pTermIdList_->end(); iter++)
//        {
//        	cout << iter->termid_<<" ";
//        }
//        cout<<endl;

        processDocumentAnalyzedContent(); // xxx
    }

    /**
     * Sub class can override this function for concrete processing.
     */
    virtual void processDocumentAnalyzedContent()
    {

    }

    /**
     * Sub class can override this function for concrete processing.
     */
    virtual void postProcess()
    {

    }

protected:
    bool isProcessProperty(izenelib::util::UString propertyName)
    {
        std::map<izenelib::util::UString, bool>::iterator iter;
        iter = processProperties_.find(propertyName);

        if (iter != processProperties_.end())
            return true;
        return false;
    }

    void createAnalyzer(bool removeStopwords=false)
    {
        IDMAnalyzerConfig common_config = IDMAnalyzerConfig::GetCommonConfig("", laResPath_, "");
        common_config.cma_config.remove_stopwords = removeStopwords;
        pIdmAnalyzer_.reset(
                new idmlib::util::IDMAnalyzer(common_config) );
        BOOST_ASSERT(pIdmAnalyzer_);
    }

    bool getScdFileList(const std::string& scdDir, std::vector<std::string>& fileList);

protected:
    CollectionPath colPath_;
    std::string laResPath_;
    size_t maxDoc_;
    izenelib::util::UString::EncodingType encoding_;
    std::map<izenelib::util::UString, bool> processProperties_;

    // Language Analyzer
    boost::shared_ptr<idmlib::util::IDMAnalyzer> pIdmAnalyzer_;
    // ID Manager
    //boost::shared_ptr<IDManager> pIdManager_;
    IDManagerESA* s_pIdManager_;

    // temporary: current document information
    uint32_t curDocId_;
    izenelib::SCDDocPtr curSCDDoc_;
    izenelib::util::UString content_;
    boost::shared_ptr<TermIdList> pTermIdList_;
};


NS_IDMLIB_UTIL_END

#endif /* COLLECTION_UTIL_H_ */
