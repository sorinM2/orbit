#pragma once
#include <string>
#include "entt/entt.hpp"
#include "orbit/utility/freelist.h"
#include "orbit/content/model.h"
#include <filesystem>
#include <memory>

namespace editor
{
    template <typename result_type = void>
    class panel
    {
    public:
        virtual void initialize() = 0;
        virtual result_type update() = 0;
        virtual void shutdown() = 0;

        virtual ~panel() = default;
    };

    class view_panel final : public panel<>
    {
        public:
        void initialize() override {}
        void update() override;
        void shutdown() override {}
    };

    class entity_panel final : public panel<>
    {
    public:
        void initialize() override {}
        void update() override;
        void shutdown() override {}

        static entt::entity selected_entity;

    };

    class selected_entity_panel final : public panel<>
    {
    public:
        void initialize() override {}
        void update() override;
        void shutdown() override {}
    };

    class model_popup;

    class model_panel final : public panel<>
    {
    public:
        void initialize() override;
        void update() override;
        void shutdown() override {}

        model_panel();
        ~model_panel() override;
        static ::orbit::content::model::handle_type selected_model;

    private:
        std::unique_ptr<model_popup> _popup;
    };

    class file_panel : public panel<std::filesystem::path>
    {
    public:
        void initialize() override {}
        std::filesystem::path update() override;
        void shutdown() override {}

        void set_path(const std::filesystem::path& path) { _path = path; }

    private:
        std::filesystem::path _path = std::filesystem::current_path().root_path();
        std::filesystem::path _selected_path;
    };
}