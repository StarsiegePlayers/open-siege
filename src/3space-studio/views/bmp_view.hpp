#ifndef DARKSTARDTSCONVERTER_BMP_VIEW_HPP
#define DARKSTARDTSCONVERTER_BMP_VIEW_HPP

#include "graphics_view.hpp"
#include "resource/resource_explorer.hpp"
#include "content/palette.hpp"

namespace studio::views
{
  class bmp_view : public graphics_view
  {
  public:
    bmp_view(const studio::resource::file_info& info, std::basic_istream<std::byte>& image_stream, const studio::resource::resource_explorer&);
    std::map<sf::Keyboard::Key, std::reference_wrapper<std::function<void(const sf::Event&)>>> get_callbacks() override;
    void setup_view(wxWindow& parent, sf::RenderWindow& window, ImGuiContext& guiContext) override;
    void render_gl(wxWindow& parent, sf::RenderWindow& window, ImGuiContext& guiContext) override {}
    void render_ui(wxWindow& parent, sf::RenderWindow& window, ImGuiContext& guiContext) override;

  private:
    static std::filesystem::path export_path;
    studio::resource::file_info info;
    const studio::resource::resource_explorer& archive;
    std::vector<content::pal::colour> default_colours;
    std::list<std::string> sort_order;
    std::map<std::string_view, std::pair<studio::resource::file_info, std::vector<content::pal::palette>>> loaded_palettes;
    std::string_view selected_palette_name;
    std::size_t selected_palette_index = std::string::npos;

    int selected_bitmap_index = 0;

    bool is_phoenix_bitmap = false;

    std::string_view default_palette_name;
    std::size_t default_palette_index = std::string::npos;

    bool opened_folder = false;

    void refresh_image();

    enum class strategy : int
    {
      do_nothing,
      remap,
      remap_unique
    };
    int colour_strategy = 1;

    std::vector<std::vector<std::byte>> original_pixels;

    sf::Image loaded_image;
    sf::Texture texture;
    sf::Sprite sprite;

    float image_scale = 1;
    bool scale_changed = false;

    std::function<void(const sf::Event&)> zoom_in;
    std::function<void(const sf::Event&)> zoom_out;
  };

}
#endif//DARKSTARDTSCONVERTER_BMP_VIEW_HPP
