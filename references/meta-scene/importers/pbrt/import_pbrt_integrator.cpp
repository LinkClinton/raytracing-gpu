#include "import_pbrt_integrator.hpp"

#include "../../integrators/bidirectional_path_integrator.hpp"
#include "../../integrators/photon_mapping_integrator.hpp"
#include "../../integrators/volume_path_integrator.hpp"
#include "../../integrators/direct_integrator.hpp"
#include "../../integrators/path_integrator.hpp"

#ifdef __PBRT_IMPORTER__

namespace metascene::importers::pbrt {

	void import_path_integrator(scene_context& context, std::shared_ptr<integrator>& integrator)
	{
		auto instance = std::make_shared<path_integrator>();

		instance->threshold = static_cast<real>(1);
		instance->depth = 5;
		
		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_INTEGER_TOKEN) {
					const auto value = context.peek_integer<size_t>();
					
					if (name == "maxdepth") META_SCENE_FINISHED_AND_RETURN(instance->depth = value);
				}

				if (type == PBRT_FLOAT_TOKEN) {
					const auto value = context.peek_real();

					if (name == "rrthreshold") META_SCENE_FINISHED_AND_RETURN(instance->threshold = value);
				}

				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		integrator = instance;
	}

	void import_vol_path_integrator(scene_context& context, std::shared_ptr<integrator>& integrator)
	{
		auto instance = std::make_shared<volume_path_integrator>();

		instance->threshold = static_cast<real>(1);
		instance->depth = 5;

		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_INTEGER_TOKEN) {
					const auto value = context.peek_integer<size_t>();

					if (name == "maxdepth") META_SCENE_FINISHED_AND_RETURN(instance->depth = value);
				}

				if (type == PBRT_FLOAT_TOKEN) {
					const auto value = context.peek_real();

					if (name == "rrthreshold") META_SCENE_FINISHED_AND_RETURN(instance->threshold = value);
				}

				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		integrator = instance;
	}

	void import_photon_mapping_integrator(scene_context& context, std::shared_ptr<integrator>& integrator)
	{
		auto instance = std::make_shared<photon_mapping_integrator>();

		instance->iterations = 64;
		instance->photons = 0;
		instance->radius = 1;
		instance->depth = 5;

		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_INTEGER_TOKEN) {
					const auto value = context.peek_integer<int>();

					if (name == "maxdepth") META_SCENE_FINISHED_AND_RETURN(instance->depth = value);
					if (name == "iterations" || name == "numiterations") META_SCENE_FINISHED_AND_RETURN(instance->iterations = value);
					if (name == "photonsperiteration") META_SCENE_FINISHED_AND_RETURN(instance->photons = value == -1 ? 0 : value);
				}

				if (type == PBRT_FLOAT_TOKEN) {
					const auto value = context.peek_real();

					if (name == "radius") META_SCENE_FINISHED_AND_RETURN(instance->radius = value);
				}

				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		integrator = instance;
	}

	void import_bidirectional_path_integrator(scene_context& context, std::shared_ptr<integrator>& integrator)
	{
		auto instance = std::make_shared<bidirectional_path_integrator>();

		instance->depth = 5;
		
		context.loop_important_token([&]()
			{
				auto [type, name] = context.peek_type_and_name();

				if (type == PBRT_INTEGER_TOKEN) {
					const auto value = context.peek_integer<int>();

					if (name == "maxdepth") META_SCENE_FINISHED_AND_RETURN(instance->depth = value);
				}
			
				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		integrator = instance;
	}
	
	void import_direct_integrator(scene_context& context, std::shared_ptr<integrator>& integrator)
	{
		auto instance = std::make_shared<direct_integrator>();

		instance->emitter_samples = 4;
		instance->bsdf_samples = 4;
		instance->depth = 5;

		context.loop_important_token([&]()
			{
				META_SCENE_PBRT_UN_RESOLVE_TOKEN;
			});

		integrator = instance;
	}
	
	void import_integrator(scene_context& context, std::shared_ptr<integrator>& integrator)
	{
		const auto integrator_type = remove_special_character(context.peek_one_token());

		std::shared_ptr<integrators::integrator> instance = nullptr;

		if (integrator_type == "bdpt") import_bidirectional_path_integrator(context, instance);
		if (integrator_type == "sppm") import_photon_mapping_integrator(context, instance);
		if (integrator_type == "directlighting") import_direct_integrator(context, instance);
		if (integrator_type == "volpath") import_vol_path_integrator(context, instance);
		if (integrator_type == "path") import_path_integrator(context, instance);

		META_SCENE_IMPORT_SUCCESS_CHECK(instance);

		integrator = instance;
	}
	
}

#endif