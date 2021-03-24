#include"dataflow.h"

using namespace TENET;
using namespace std;

int test_PEArray(shared_ptr<ISL_Context> context)
{
	const char *pe_domain_str = "{PE[i,j]:0<=i,j<4}";
	const char *interconnect_str =
		"{PE[i,j]->PE[i+1,j];PE[i,j]->PE[i-1,j];PE[i,j]->PE[i,j+1];PE[i,j]->PE[i,j-1]}";

	PEArray a1(context, pe_domain_str, interconnect_str,256,1024,256,16);
	PEArray a2 = a1.copy();
	isl_union_set * domain = a1.GetDomain();
	isl_union_map * interconnect = a2.GetInterconnect();
	context->printer(isl_printer_print_str, "PE Domain:\n");
	context->printer(isl_printer_print_union_set, domain);
	context->printer(isl_printer_print_str, "\nSuggested: {PE[i,j]:0<=i,j<4} \nInterconnect:\n");
	context->printer(isl_printer_print_union_map, interconnect);
	context->printer(isl_printer_print_str, "\nSuggested: PE[i,j] to adjacent PE in domain\n");
	isl_union_set_free(domain);
	isl_union_map_free(interconnect);
	return 0;
}

int test_access(shared_ptr<ISL_Context> context)
{
	Access a1(context, "C", "{[i,j,k]->C[i,k]}", true);
	Access a2 = a1.copy();
	Access a3(context, "A", "{[i,j,k]->A[i,j]}", false);
	a3 = a1.copy();
	isl_union_map *access = a1.GetAccess();
	context->printer(isl_printer_print_union_map, access);
	context->printer(isl_printer_end_line);
	isl_union_map * access2 = a3.GetAccess();
	context->printer(isl_printer_print_union_map, access2);
	context->printer(isl_printer_end_line);
	isl_union_map_free(access);
	isl_union_map_free(access2);
	return 0;
}

int test_statement(shared_ptr<ISL_Context> context)
{
	Access c1(context, "C", "{S[i,j,k]->C[i,k]}", true);
	Access c2(context, "C", "{S[i,j,k]->C[i,k]}", false);
	Access a1(context, "A", "{S[i,j,k]->A[i,j]}", false);
	Access b1(context, "B", "{S[i,j,k]->B[j,k]}", false);
	Statement s(context, "[n]->{S[i,j,k]:0<=i,j,k<n}");

	s.AddAccess(move(c1));
	s.AddAccess(move(c2));
	s.AddAccess(move(a1));
	s.AddAccess(move(b1));

	Statement s2 = s.copy();
	Statement s3 = s.copy();
	isl_union_set *domain = s.GetDomain();
	context->printer(isl_printer_print_union_set, domain);
	context->printer(isl_printer_end_line);
	isl_union_map *acc_c = s.GetAccess("C", AccessType::READ_OR_WRITE);
	context->printer(isl_printer_print_union_map, acc_c);
	context->printer(isl_printer_end_line);
	isl_union_map *acc_a = s.GetAccess("A", AccessType::READ);
	context->printer(isl_printer_print_union_map, acc_a);
	context->printer(isl_printer_end_line);
	isl_union_map_free(acc_a);
	isl_union_map_free(acc_c);
	isl_union_set_free(domain);
	return 0;
}

int test_dataflow(shared_ptr<ISL_Context> context)
{
	const char *pe_domain_str = "{PE[i]:0<=i<6}";
	const char *interconnect_str = "{PE[i]->PE[i+1];PE[i]->PE[i-1]}";
	PEArray pe(context, pe_domain_str, interconnect_str, 256, 1024, 256,16);
	Access o(context, "O", "{S[x,s]->O[x]}", true);
	Access o2(context, "O", "{S[x,s]->O[x]}", false);
	Access w(context, "W", "{S[x,s]->W[s]}", false);
	Access i(context, "I", "{S[x,s]->I[x+s]}", false);
	Statement s(context, "{S[x,s]:0<=x<12 and 0<=s < 6}");
	s.AddAccess(move(o));
	s.AddAccess(move(o2));
	s.AddAccess(move(w));
	s.AddAccess(move(i));

	Mapping m(context, "{S[x,s]->PE[s]}", "{S[x,s]->T[x]}");
	Dataflow df(move(s), move(pe), move(m));

	context->printer(isl_printer_print_str, "\nSpaceMap:\n");
	isl_union_map *space_map = df.GetSpaceMap();
	context->printer(isl_printer_print_union_map, space_map);

	context->printer(isl_printer_print_str, "\nTimeMap:\n");
	isl_union_map *time_map = df.GetTimeMap();
	context->printer(isl_printer_print_union_map, time_map);

	context->printer(isl_printer_print_str, "\nSpaceTimeMap:\n");
	isl_union_map *space_time_map = df.GetSpaceTimeMap();
	context->printer(isl_printer_print_union_map, space_time_map);

	context->printer(isl_printer_print_str, "\nDomain:\n");
	isl_union_set *domain = df.GetDomain();
	context->printer(isl_printer_print_union_set, domain);

	context->printer(isl_printer_print_str, "\nSpaceDomain:\n");
	isl_union_set *space_domain = df.GetSpaceDomain();
	context->printer(isl_printer_print_union_set, space_domain);

	context->printer(isl_printer_print_str, "\nTimeDomain:\n");
	isl_union_set *time_domain = df.GetTimeDomain();
	context->printer(isl_printer_print_union_set, time_domain);

	context->printer(isl_printer_print_str, "\nSpaceTimeDomain:\n");
	isl_union_set *space_time_domain = df.GetSpaceTimeDomain();
	context->printer(isl_printer_print_union_set, space_time_domain);

	context->printer(isl_printer_print_str, "\nTimeToPrev:\n");
	isl_union_map *time_to_prev = df.MapTimeToPrev(1, false);
	context->printer(isl_printer_print_union_map, time_to_prev);

	context->printer(isl_printer_print_str, "\nSpaceToNeighbor:\n");
	isl_union_map *space_to_neighbor = df.MapSpaceToNeighbor(1, false);
	context->printer(isl_printer_print_union_map, space_to_neighbor);

	context->printer(isl_printer_print_str, "\nSpaceTimeToNeighbor:\n");
	isl_union_map *space_time_to_neighbor = df.MapSpaceTimeToNeighbor(1, true, 1, true, false);
	context->printer(isl_printer_print_union_map, space_time_to_neighbor);

	context->printer(isl_printer_print_str, "\nSpaceTimeToAccess:\n");
	isl_union_map *space_time_to_access = df.MapSpaceTimeToAccess("W", AccessType::READ);
	context->printer(isl_printer_print_union_map, space_time_to_access);
	context->printer(isl_printer_end_line);

	isl_union_map_free(space_map);
	isl_union_map_free(time_map);
	isl_union_map_free(space_time_map);
	isl_union_set_free(domain);
	isl_union_set_free(space_domain);
	isl_union_set_free(time_domain);
	isl_union_set_free(space_time_domain);
	isl_union_map_free(time_to_prev);
	isl_union_map_free(space_to_neighbor);
	isl_union_map_free(space_time_to_neighbor);
	isl_union_map_free(space_time_to_access);
	return 0;
}

int test_dataflow_analysis(shared_ptr<ISL_Context> context)
{
	const char *pe_domain_str = "{PE[i]:0<=i<3}";
	const char *interconnect_str = "{PE[i]->PE[i+1]}";
	PEArray pe(context, pe_domain_str, interconnect_str, 256, 1024, 256,16);
	Access o(context, "O", "{S[x,s]->O[x]}", true);
	Access o2(context, "O", "{S[x,s]->O[x]}", false);
	Access w(context, "W", "{S[x,s]->W[s]}", false);
	Access i(context, "I", "{S[x,s]->I[x+s]}", false);
	Statement s(context, "{S[x,s]:0<=x<3 and 0<=s <3}");
	s.AddAccess(move(o));
	s.AddAccess(move(o2));
	s.AddAccess(move(w));
	s.AddAccess(move(i));
	Mapping m(context, "{S[x,s]->PE[s]}", "{S[x,s]->T[x]}");
	Dataflow df(move(s), move(pe), move(m));
	Dataflow ana = df.copy();
	isl_union_map *neighbor = df.MapSpaceTimeToNeighbor(1, true, 1, true, false);
	int uvo = ana.GetUniqueVolume("O", AccessType::READ_OR_WRITE,
		isl_union_map_copy(neighbor));
	int uvi = ana.GetUniqueVolume("I", AccessType::READ,
		isl_union_map_copy(neighbor));
	int uvw = ana.GetUniqueVolume("W", AccessType::READ,
		neighbor);
	int vo = ana.GetTotalVolume("O", AccessType::READ_OR_WRITE);
	int vi = ana.GetTotalVolume("I", AccessType::READ_OR_WRITE);
	int vw = ana.GetTotalVolume("W", AccessType::READ_OR_WRITE);
	fprintf(stdout, "UV[O]:%d UV[I]:%d UV[W]:%d\n", uvo,uvi,uvw);
	fprintf(stdout, "V[O]:%d V[I]:%d V[W]:%d\n", vo, vi, vw);
	return 0;
}

int test_dataload(shared_ptr<ISL_Context> context, const char* pe_file, const char* mapping_file, const char* statement_file)
{
	PEArray pe(context);
	Statement st(context);
	Mapping mp(context);
	if (pe.Load(pe_file))
		pe.PrintInfo();
	else
		cout << "pe load failed" << endl;
	if (st.Load(statement_file))
		st.PrintInfo();
	else
		cout << "statement load failed" << endl;
	if (mp.Load(mapping_file))
		mp.PrintInfo();
	else
		cout << "mapping load failed" << endl;
	return 0;
}