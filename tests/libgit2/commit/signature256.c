#include "clar_libgit2.h"

static git_repository *g_repo;

static const char *unsigned_data =
"tree f2a108f86a3b4fd9ad75ed55e9cb3cb46e348fca3b9dba3db64f7c9f64b8a736\n\
parent decaff3051968d1f3a2defd3d4a80ced03101555e1fd8913b3544026c0717d4f\n\
author Ben Burkert <ben@benburkert.com> 1358451456 -0800\n\
committer Ben Burkert <ben@benburkert.com> 1358451456 -0800\n\
\n\
a simple commit which works\n";

static const char *sig = "magic word: pretty please";

static const char *signed_data =
"tree f2a108f86a3b4fd9ad75ed55e9cb3cb46e348fca3b9dba3db64f7c9f64b8a736\n\
parent decaff3051968d1f3a2defd3d4a80ced03101555e1fd8913b3544026c0717d4f\n\
author Ben Burkert <ben@benburkert.com> 1358451456 -0800\n\
committer Ben Burkert <ben@benburkert.com> 1358451456 -0800\n\
gpgsig-sha256 magic word: pretty please\n\
\n\
a simple commit which works\n";

void test_commit_signature256__initialize(void)
{
	g_repo = cl_git_sandbox_init("testrepo_256.git");
}

void test_commit_signature256__cleanup(void)
{
	cl_git_sandbox_cleanup();
}

void test_commit_signature256__default_field_is_gpgsig_sha256(void)
{
	git_oid one, two;
	git_odb *odb;
	git_odb_object *obj;
	git_buf signature = GIT_BUF_INIT, data = GIT_BUF_INIT;

	cl_git_pass(git_commit_create_with_signature(&one, g_repo, unsigned_data, sig, NULL));
	cl_git_pass(git_commit_create_with_signature(&two, g_repo, unsigned_data, sig, "gpgsig-sha256"));
	cl_assert(!git_oid_cmp(&one, &two));

	cl_git_pass(git_repository_odb(&odb, g_repo));
	cl_git_pass(git_odb_read(&obj, odb, &one));
	cl_assert_equal_s(signed_data, git_odb_object_data(obj));

	cl_git_pass(git_commit_extract_signature(&signature, &data, g_repo, &one, NULL));
	cl_assert_equal_s(sig, signature.ptr);
	cl_assert_equal_s(unsigned_data, data.ptr);

	cl_git_fail_with(GIT_ENOTFOUND,
		git_commit_extract_signature(&signature, &data, g_repo, &one, "gpgsig"));

	git_buf_dispose(&signature);
	git_buf_dispose(&data);
	git_odb_object_free(obj);
	git_odb_free(odb);
}
