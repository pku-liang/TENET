#pragma once

#include <memory>

#include<isl/val.h>
#include<barvinok/barvinok.h>

namespace TENET
{
    struct _ctx_delete{
        void operator()(isl_ctx *p) const noexcept
        {
            isl_ctx_free(p);
        }
    };
    using isl_ctx_ptr = std::unique_ptr<isl_ctx, _ctx_delete>;

	struct _union_set_delete
	{
		void operator()(isl_union_set *p) const noexcept
        {
            isl_union_set_free(p);
        }
	};
    using isl_union_set_ptr = std::unique_ptr<isl_union_set, _union_set_delete>;

	struct _union_map_delete
	{
		void operator()(isl_union_map *p) const noexcept
        {
            isl_union_map_free(p);
        }
	};
    using isl_union_map_ptr = std::unique_ptr<isl_union_map, _union_map_delete>;
}