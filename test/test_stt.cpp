#include"dataflow.h"

int test_PEArray(isl_ctx *ctx, isl_printer *p)
{
	const char *pe_domain_str = "{PE[i,j]:0<=i,j<4}";
	const char *interconnect_str =
		"{PE[i,j]->PE[i+1,j];PE[i,j]->PE[i-1,j];PE[i,j]->PE[i,j+1];PE[i,j]->PE[i,j-1]}";
	PEArray a1(pe_domain_str, interconnect_str,256,1024,256,16);
	PEArray a2 = a1;
	isl_union_set * domain = a1.GetDomain();
	isl_union_map * interconnect = a2.GetInterconnect();
	p = isl_printer_print_str(p, "PE Domain:\n");
	p = isl_printer_print_union_set(p, domain);
	p = isl_printer_print_str(p, "\nSuggested: {PE[i,j]:0<=i,j<4} \nInterconnect:\n");
	p = isl_printer_print_union_map(p, interconnect);
	p = isl_printer_print_str(p, "\nSuggested: PE[i,j] to adjacent PE in domain\n");
	isl_union_set_free(domain);
	isl_union_map_free(interconnect);
	return 0;
}

int test_access(isl_ctx *ctx, isl_printer *p)
{
	Access a1("C", "{[i,j,k]->C[i,k]}", true);
	Access a2 = a1;
	Access a3("A", "{[i,j,k]->A[i,j]}", false);
	a3 = a1;
	isl_union_map *access = a1.GetAccess();
	p = isl_printer_print_union_map(p, access);
	p = isl_printer_end_line(p);
	isl_union_map * access2 = a3.GetAccess();
	p = isl_printer_print_union_map(p, access2);
	p = isl_printer_end_line(p);
	isl_union_map_free(access);
	isl_union_map_free(access2);
	return 0;
}

int test_statement(isl_ctx *ctx, isl_printer *p)
{
	Access c1("C", "{S[i,j,k]->C[i,k]}", true);
	Access c2("C", "{S[i,j,k]->C[i,k]}", false);
	Access a1("A", "{S[i,j,k]->A[i,j]}", false);
	Access b1("B", "{S[i,j,k]->B[j,k]}", false);
	Statement s("[n]->{S[i,j,k]:0<=i,j,k<n}");
	
	s.AddAccess(c1);
	s.AddAccess(c2);
	s.AddAccess(a1);
	s.AddAccess(b1);
	Statement s2(s);
	Statement s3 = s;
	isl_union_set *domain = s.GetDomain();
	p = isl_printer_print_union_set(p, domain);
	p = isl_printer_end_line(p);
	isl_union_map *acc_c = s.GetAccess("C", READ_OR_WRITE);
	p = isl_printer_print_union_map(p, acc_c);
	p = isl_printer_end_line(p);
	isl_union_map *acc_a = s.GetAccess("A", READ);
	p = isl_printer_print_union_map(p, acc_a);
	p = isl_printer_end_line(p);
	isl_union_map_free(acc_a);
	isl_union_map_free(acc_c);
	isl_union_set_free(domain);
	return 0;
}

int test_dataflow(isl_ctx *ctx, isl_printer *p)
{
	const char *pe_domain_str = "{PE[i]:0<=i<6}";
	const char *interconnect_str = "{PE[i]->PE[i+1];PE[i]->PE[i-1]}";
	PEArray pe(pe_domain_str, interconnect_str, 256, 1024, 256,16);
	Access o("O", "{S[x,s]->O[x]}", true);
	Access o2("O", "{S[x,s]->O[x]}", false);
	Access w("W", "{S[x,s]->W[s]}", false);
	Access i("I", "{S[x,s]->I[x+s]}", false);
	Statement s("{S[x,s]:0<=x<12 and 0<=s < 6}");
	s.AddAccess(o);
	s.AddAccess(o2);
	s.AddAccess(w);
	s.AddAccess(i);
	Mapping m("{S[x,s]->PE[s]}", "{S[x,s]->T[x]}");
	Dataflow df(s, pe,m);
	
	p = isl_printer_print_str(p, "\nSpaceMap:\n");
	isl_union_map *space_map = df.GetSpaceMap();
	p = isl_printer_print_union_map(p, space_map);
	
	p = isl_printer_print_str(p, "\nTimeMap:\n");
	isl_union_map *time_map = df.GetTimeMap();
	p = isl_printer_print_union_map(p, time_map);
	
	p = isl_printer_print_str(p, "\nSpaceTimeMap:\n");
	isl_union_map *space_time_map = df.GetSpaceTimeMap();
	p = isl_printer_print_union_map(p, space_time_map);

	p = isl_printer_print_str(p, "\nDomain:\n");
	isl_union_set *domain = df.GetDomain();
	p = isl_printer_print_union_set(p, domain);

	p = isl_printer_print_str(p, "\nSpaceDomain:\n");
	isl_union_set *space_domain = df.GetSpaceDomain();
	p = isl_printer_print_union_set(p, space_domain);

	p = isl_printer_print_str(p, "\nTimeDomain:\n");
	isl_union_set *time_domain = df.GetTimeDomain();
	p = isl_printer_print_union_set(p, time_domain);

	p = isl_printer_print_str(p, "\nSpaceTimeDomain:\n");
	isl_union_set *space_time_domain = df.GetSpaceTimeDomain();
	p = isl_printer_print_union_set(p, space_time_domain);

	p = isl_printer_print_str(p, "\nTimeToPrev:\n");
	isl_union_map *time_to_prev = df.MapTimeToPrev(1, false);
	p = isl_printer_print_union_map(p, time_to_prev);

	p = isl_printer_print_str(p, "\nSpaceToNeighbor:\n");
	isl_union_map *space_to_neighbor = df.MapSpaceToNeighbor(1, false);
	p = isl_printer_print_union_map(p, space_to_neighbor);

	p = isl_printer_print_str(p, "\nSpaceTimeToNeighbor:\n");
	isl_union_map *space_time_to_neighbor = df.MapSpaceTimeToNeighbor(1, true, 1, true, false);
	p = isl_printer_print_union_map(p, space_time_to_neighbor);
	
	p = isl_printer_print_str(p, "\nSpaceTimeToAccess:\n");	
	isl_union_map *space_time_to_access = df.MapSpaceTimeToAccess("W", READ);
	p = isl_printer_print_union_map(p, space_time_to_access);
	p = isl_printer_end_line(p);

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

int test_dataflow_analysis(isl_ctx *ctx, isl_printer *p)
{
	const char *pe_domain_str = "{PE[i]:0<=i<3}";
	const char *interconnect_str = "{PE[i]->PE[i+1]}";
	PEArray pe(pe_domain_str, interconnect_str, 256, 1024, 256,16);
	Access o("O", "{S[x,s]->O[x]}", true);
	Access o2("O", "{S[x,s]->O[x]}", false);
	Access w("W", "{S[x,s]->W[s]}", false);
	Access i("I", "{S[x,s]->I[x+s]}", false);
	Statement s("{S[x,s]:0<=x<3 and 0<=s <3}");
	s.AddAccess(o);
	s.AddAccess(o2);
	s.AddAccess(w);
	s.AddAccess(i);
	Mapping m("{S[x,s]->PE[s]}", "{S[x,s]->T[x]}");
	Dataflow df(s, pe, m);
	DataflowAnalysis ana(df);
	isl_union_map *neighbor = df.MapSpaceTimeToNeighbor(1, true, 1, true, false);
	int uvo = ana.GetUniqueVolume("O", READ_OR_WRITE, 
		isl_union_map_copy(neighbor));
	int uvi = ana.GetUniqueVolume("I", READ,
		isl_union_map_copy(neighbor));
	int uvw = ana.GetUniqueVolume("W", READ,
		neighbor);
	int vo = ana.GetTotalVolume("O", READ_OR_WRITE);
	int vi = ana.GetTotalVolume("I", READ_OR_WRITE);
	int vw = ana.GetTotalVolume("W", READ_OR_WRITE);
	fprintf(stdout, "UV[O]:%d UV[I]:%d UV[W]:%d\n", uvo,uvi,uvw);
	fprintf(stdout, "V[O]:%d V[I]:%d V[W]:%d\n", vo, vi, vw);
	return 0;
}

int test_dataload(const char* pe_file, const char* mapping_file, const char* statement_file)
{
	PEArray pe;
	Statement st;
	Mapping mp;
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