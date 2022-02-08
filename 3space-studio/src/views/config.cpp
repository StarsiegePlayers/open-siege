#include "config.hpp"
#include "darkstar_dts_view.hpp"
#include "bmp_view.hpp"
#include "pal_view.hpp"
#include "vol_view.hpp"
#include "sfx_view.hpp"
#include "content/dts/3space.hpp"
#include "content/mis/mission.hpp"
#include "content/dts/darkstar.hpp"
#include "content/bmp/bitmap.hpp"
#include "content/sfx/wave.hpp"
#include "resources/darkstar_volume.hpp"
#include "resources/three_space_volume.hpp"
#include "resources/trophy_bass_volume.hpp"

namespace dio
{
  namespace mis = studio::resources::mis;
  namespace vol = studio::resources::vol;
}

namespace studio::views
{
  view_factory create_default_view_factory()
  {
    view_factory view_factory;
    view_factory.add_file_type(content::dts::darkstar::is_darkstar_dts, [](auto& info, auto& stream, auto& manager) { return std::unique_ptr<studio_view>(new darkstar_dts_view(info, stream, manager)); });
    view_factory.add_file_type(content::dts::three_space::v1::is_3space_dts, [](auto& info, auto& stream, auto& manager) { return std::unique_ptr<studio_view>(new darkstar_dts_view(info, stream, manager)); });
    view_factory.add_file_type(content::bmp::is_microsoft_bmp, [](auto& info, auto& stream, auto& manager) { return std::unique_ptr<studio_view>(new bmp_view(info, stream, manager)); });
    view_factory.add_file_type(content::bmp::is_phoenix_bmp, [](auto& info, auto& stream, auto& manager) { return std::unique_ptr<studio_view>(new bmp_view(info, stream, manager)); });
    view_factory.add_file_type(content::bmp::is_earthsiege_bmp, [](auto& info, auto& stream, auto& manager) { return std::unique_ptr<studio_view>(new bmp_view(info, stream, manager)); });
    view_factory.add_file_type(content::bmp::is_earthsiege_bmp_array, [](auto& info, auto& stream, auto& manager) { return std::unique_ptr<studio_view>(new bmp_view(info, stream, manager)); });
    view_factory.add_file_type(content::bmp::is_phoenix_bmp_array, [](auto& info, auto& stream, auto& manager) { return std::unique_ptr<studio_view>(new bmp_view(info, stream, manager)); });
    view_factory.add_file_type(content::pal::is_old_pal, [](auto&, auto& stream, auto&) { return std::unique_ptr<studio_view>(new pal_view(stream)); });
    view_factory.add_file_type(content::pal::is_microsoft_pal, [](auto&, auto& stream, auto&) { return std::unique_ptr<studio_view>(new pal_view(stream)); });
    view_factory.add_file_type(content::pal::is_phoenix_pal, [](auto&, auto& stream, auto&) { return std::unique_ptr<studio_view>(new pal_view(stream)); });
    view_factory.add_file_type(content::pal::is_earthsiege_pal, [](auto&, auto& stream, auto&) { return std::unique_ptr<studio_view>(new pal_view(stream)); });

    view_factory.add_file_type(content::sfx::is_sfx_file, [](auto& info, auto& stream, auto& archive) { return std::unique_ptr<studio_view>(new sfx_view(info, stream, archive)); }, true);

    view_factory.add_file_type(dio::mis::darkstar::mis_file_archive::is_supported, [](auto& info, auto&, auto& archive) { return std::unique_ptr<studio_view>(new vol_view(info, archive)); });
    view_factory.add_file_type(dio::vol::darkstar::vol_file_archive::is_supported, [](auto& info, auto&, auto& archive) { return std::unique_ptr<studio_view>(new vol_view(info, archive)); });

    view_factory.add_file_type(dio::vol::three_space::vol_file_archive::is_supported, [](auto& info, auto&, auto& archive) { return std::unique_ptr<studio_view>(new vol_view(info, archive)); });
    view_factory.add_file_type(dio::vol::three_space::rmf_file_archive::is_supported, [](auto& info, auto&, auto& archive) { return std::unique_ptr<studio_view>(new vol_view(info, archive)); });
    view_factory.add_file_type(dio::vol::three_space::dyn_file_archive::is_supported, [](auto& info, auto&, auto& archive) { return std::unique_ptr<studio_view>(new vol_view(info, archive)); });
    view_factory.add_file_type(dio::vol::trophy_bass::rbx_file_archive::is_supported, [](auto& info, auto&, auto& archive) { return std::unique_ptr<studio_view>(new vol_view(info, archive)); });
    view_factory.add_file_type(dio::vol::trophy_bass::tbv_file_archive::is_supported, [](auto& info, auto&, auto& archive) { return std::unique_ptr<studio_view>(new vol_view(info, archive)); });

    view_factory.add_extension(".dts", content::dts::darkstar::is_darkstar_dts);
    view_factory.add_extension(".dts", content::dts::three_space::v1::is_3space_dts);

    view_factory.add_extension(".bmp", content::bmp::is_microsoft_bmp);
    view_factory.add_extension(".bmp", content::bmp::is_phoenix_bmp);
    view_factory.add_extension(".dib", content::bmp::is_microsoft_bmp);
    view_factory.add_extension(".dib", content::bmp::is_phoenix_bmp);
    view_factory.add_extension(".pba", content::bmp::is_phoenix_bmp_array);
    view_factory.add_extension(".dbm", content::bmp::is_earthsiege_bmp);
    view_factory.add_extension(".dba", content::bmp::is_earthsiege_bmp_array);
    view_factory.add_extension(".db0", content::bmp::is_earthsiege_bmp_array);
    view_factory.add_extension(".db1", content::bmp::is_earthsiege_bmp_array);
    view_factory.add_extension(".db2", content::bmp::is_earthsiege_bmp_array);
    view_factory.add_extension(".hba", content::bmp::is_earthsiege_bmp_array);
    view_factory.add_extension(".hb0", content::bmp::is_earthsiege_bmp_array);
    view_factory.add_extension(".hb1", content::bmp::is_earthsiege_bmp_array);
    view_factory.add_extension(".hb2", content::bmp::is_earthsiege_bmp_array);

    view_factory.add_extension(".sfx", content::sfx::is_sfx_file);

    view_factory.add_extension(".pal", content::pal::is_old_pal);
    view_factory.add_extension(".pal", content::pal::is_microsoft_pal);
    view_factory.add_extension(".ipl", content::pal::is_microsoft_pal);
    view_factory.add_extension(".ppl", content::pal::is_microsoft_pal);
    view_factory.add_extension(".pal", content::pal::is_phoenix_pal);
    view_factory.add_extension(".ipl", content::pal::is_phoenix_pal);
    view_factory.add_extension(".ppl", content::pal::is_phoenix_pal);
    view_factory.add_extension(".dpl", content::pal::is_earthsiege_pal);

    view_factory.add_extension(".mis", dio::mis::darkstar::mis_file_archive::is_supported);
    view_factory.add_extension(".vol", dio::vol::darkstar::vol_file_archive::is_supported);

    view_factory.add_extension(".vol", dio::vol::three_space::vol_file_archive::is_supported);
    view_factory.add_extension(".rmf", dio::vol::three_space::rmf_file_archive::is_supported);
    view_factory.add_extension(".map", dio::vol::three_space::rmf_file_archive::is_supported);
    view_factory.add_extension(".vga", dio::vol::three_space::rmf_file_archive::is_supported);

    view_factory.add_extension(".dyn", dio::vol::three_space::dyn_file_archive::is_supported);
    view_factory.add_extension(".rbx", dio::vol::trophy_bass::rbx_file_archive::is_supported);
    view_factory.add_extension(".tbv", dio::vol::trophy_bass::tbv_file_archive::is_supported);

    return view_factory;
  }

  studio::resources::resource_explorer create_default_resource_explorer(const std::filesystem::path& search_path)
  {
    studio::resources::resource_explorer archive(search_path);

    archive.add_archive_type(".mis", std::make_unique<dio::mis::darkstar::mis_file_archive>(), dio::mis::darkstar::mis_file_archive::supported_extensions);
    archive.add_archive_type(".tbv", std::make_unique<dio::vol::trophy_bass::tbv_file_archive>());
    archive.add_archive_type(".rbx", std::make_unique<dio::vol::trophy_bass::rbx_file_archive>());
    archive.add_archive_type(".rmf", std::make_unique<dio::vol::three_space::rmf_file_archive>());
    archive.add_archive_type(".map", std::make_unique<dio::vol::three_space::rmf_file_archive>());
    archive.add_archive_type(".vga", std::make_unique<dio::vol::three_space::rmf_file_archive>());

    archive.add_archive_type(".dyn", std::make_unique<dio::vol::three_space::dyn_file_archive>());
    archive.add_archive_type(".vol", std::make_unique<dio::vol::three_space::vol_file_archive>());
    archive.add_archive_type(".vol", std::make_unique<dio::vol::darkstar::vol_file_archive>());

    return archive;
  }
}