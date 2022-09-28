/*
 * Copyright (c) 1994 by Sun Microsystems, Inc.
 * All rights reserved.
 */

#pragma ident "@(#)FNSP_OrgContext.hh	1.3 94/11/20 SMI"

#ifndef _FNSP_ORGCONTEXT_HH
#define	_FNSP_ORGCONTEXT_HH

#include <xfn/fn_spi.hh>

/* For context type OrganizationName */
class FNSP_OrgContext : public FN_ctx_csvc_strong {
    private:
	FN_ref *my_reference;  /* encoded */

	// internal functions
	FNSP_OrgContext(const FN_ref_addr&, const FN_ref&);
	const FN_ref* resolve(const FN_string&, FN_status_csvc&);
	FN_ref *make_nsid_ref();
	FN_ref *make_service_ref();

    public:
	~FNSP_OrgContext();

#ifdef DEBUG
	// only used for testing
	FNSP_OrgContext(const FN_identifier *addr_type, const FN_string&);
	FNSP_OrgContext(const FN_ref&);
#endif /* DEBUG */

	FN_ref* get_ref(FN_status&) const;
	static FNSP_OrgContext* from_address(
	    const FN_ref_addr&, const FN_ref&, FN_status& stat);


	// implementations FN_ctx_csvc_strong
	FN_ref* c_lookup(const FN_string&, unsigned int f, FN_status_csvc&);
	FN_namelist* c_list_names(const FN_string& name, FN_status_csvc&);
	FN_bindinglist* c_list_bindings(const FN_string& name, FN_status_csvc&);
	int c_bind(const FN_string& name, const FN_ref&,
				    unsigned BindFlags, FN_status_csvc&);
	int c_unbind(const FN_string& name, FN_status_csvc&);
	int c_rename(const FN_string& name, const FN_composite_name&,
					unsigned BindFlags, FN_status_csvc&);
	FN_ref* c_create_subcontext(const FN_string& name, FN_status_csvc&);
	int c_destroy_subcontext(const FN_string& name, FN_status_csvc&);
	FN_attrset* c_get_syntax_attrs(const FN_string& name, FN_status_csvc&);

	// Attribute Operations
	FN_attribute *c_attr_get(const FN_string&,
					const FN_identifier&, FN_status_csvc&);
	int c_attr_modify(const FN_string&,
		unsigned int, const FN_attribute&, FN_status_csvc&);
	FN_valuelist *c_attr_get_values(const FN_string&,
				    const FN_identifier&, FN_status_csvc&);
	FN_attrset *c_attr_get_ids(const FN_string&, FN_status_csvc&);
	FN_multigetlist *c_attr_multi_get(const FN_string&,
	    const FN_attrset *, FN_status_csvc&);
	int c_attr_multi_modify(const FN_string&,
	    const FN_attrmodlist&, FN_attrmodlist **, FN_status_csvc&);

	FN_ref* c_lookup_nns(const FN_string& name,
				    unsigned int f, FN_status_csvc&);
	FN_namelist* c_list_names_nns(const FN_string& name, FN_status_csvc&);
	FN_bindinglist* c_list_bindings_nns(const FN_string& name,
	    FN_status_csvc&);
	int c_bind_nns(const FN_string& name, const FN_ref&,
				    unsigned BindFlags, FN_status_csvc&);
	int c_unbind_nns(const FN_string& name, FN_status_csvc&);
	int c_rename_nns(const FN_string& name, const FN_composite_name&,
					unsigned BindFlags, FN_status_csvc&);
	FN_ref* c_create_subcontext_nns(const FN_string& name, FN_status_csvc&);
	int c_destroy_subcontext_nns(const FN_string& name, FN_status_csvc&);
	FN_attrset* c_get_syntax_attrs_nns(const FN_string& name,
	    FN_status_csvc&);

	// Attribute Operations
	FN_attribute *c_attr_get_nns(const FN_string&,
				    const FN_identifier&, FN_status_csvc&);
	int c_attr_modify_nns(const FN_string&,
	    unsigned int, const FN_attribute&, FN_status_csvc&);
	FN_valuelist *c_attr_get_values_nns(const FN_string&,
				    const FN_identifier&, FN_status_csvc&);
	FN_attrset *c_attr_get_ids_nns(const FN_string&, FN_status_csvc&);
	FN_multigetlist *c_attr_multi_get_nns(const FN_string&,
	    const FN_attrset *, FN_status_csvc&);
	int c_attr_multi_modify_nns(const FN_string&,
	    const FN_attrmodlist&, FN_attrmodlist **, FN_status_csvc&);
};

#endif _FNSP_ORGCONTEXT_HH
