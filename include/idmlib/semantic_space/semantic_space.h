/**
 * @file idmlib/semantic_space/semantic_interpreter.h
 * @author Zhongxia Li
 * @date Mar 10, 2011
 * @brief Vector space base
 */
#ifndef SEMANTIC_SPACE_H_
#define SEMANTIC_SPACE_H_

#include <set>
#include <iostream>

#include <idmlib/idm_types.h>
#include <idmlib/semantic_space/term_doc_matrix_defs.h>
#include <idmlib/util/FSUtil.hpp>
#include <ir/index_manager/index/LAInput.h>

using namespace izenelib::ir::indexmanager;

NS_IDMLIB_SSP_BEGIN

class SemanticSpace
{
public:
	enum eSSPInitType {
		CREATE, // Create new semantic space data
		LOAD    // Load from existed data
	};

public:
	SemanticSpace(
			const std::string& sspPath,
			SemanticSpace::eSSPInitType initType = SemanticSpace::CREATE)
	: sspPath_(sspPath)
	{
		idmlib::util::FSUtil::normalizeFilePath(sspPath_);

		if (initType ==  SemanticSpace::CREATE) {
		    DLOG(INFO) << "Create Semantic Space Data." << endl;
			// clear existed files
			idmlib::util::FSUtil::del(sspPath_);
		}

		term2dfFile_.reset(new term_df_file(sspPath_ + "/term_df_map"));
		doclistFile_.reset(new doc_list_file(sspPath_ + "/doc_list"));
		if (initType == SemanticSpace::LOAD) {
			term2dfFile_->Load();
			termid2df_ = term2dfFile_->GetValue();
			doclistFile_->Load();
			docList_ = doclistFile_->GetValue();
			DLOG(INFO) << "Load Semantic Space Data: [doc " << docList_.size() << ", term " << termid2df_.size() << "]" << endl;
		}
		cout << sspPath_ << endl;
	}

	virtual ~SemanticSpace() {}

public:
	/**
	 * @defgroup Common interfaces for building semantic space
	 * @{
	 */

	/**
	 * @brief Incrementally process documents one by one.
	 * @param docid document id
	 * @param termids id of terms in the document, such as terms generated by LA.
	 */
	virtual void ProcessDocument(docid_t& docid, TermIdList& termIdList,
	        IdmTermList& termList = NULLTermList) = 0;

	/**
	 * @brief Post process after all documents are processed.
	 */
	virtual void ProcessSpace() = 0;

	virtual void SaveSpace()
	{
		term2dfFile_->SetValue(termid2df_);
		term2dfFile_->Save();
		doclistFile_->SetValue(docList_);
		doclistFile_->Save();
	}

	/** @} */


	virtual count_t getDocNum() { return 0; }

	std::vector<docid_t>& getDocList()
	{
		return docList_;
	}

	/// Available if it's forward(doc) index
	virtual void getVectorByDocid(docid_t& docid, term_sp_vector& termsVec) {}

	/// Available if it's inverted(term) index
	virtual void getVectorByTermid(termid_t& termid, doc_sp_vector& docsVec) {}

	virtual void Print() {}

protected:
	std::string sspPath_;

	// statistics of whole collection, permanent
	typedef std::map<termid_t, weight_t> term_df_map;
	term_df_map termid2df_;
	std::vector<docid_t> docList_;

	typedef izenelib::util::FileObject<term_df_map> term_df_file;
	boost::shared_ptr<term_df_file> term2dfFile_;
	typedef izenelib::util::FileObject<std::vector<docid_t> > doc_list_file;
	boost::shared_ptr<doc_list_file> doclistFile_;
};

NS_IDMLIB_SSP_END

#endif /* SEMANTIC_SPACE_H_ */
