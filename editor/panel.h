#pragma once
#include <string>
#include "entt/entt.hpp"
#include "orbit/utility/freelist.h"
#include "orbit/content/model.h"

namespace editor
{
    class panel
    {
    public:
        virtual void initialize() = 0;
        virtual void update() = 0;
        virtual void shutdown() = 0;

        virtual ~panel() = default;
    };

    class view_panel final : public panel
    {
        public:
        void initialize() override {}
        void update() override;
        void shutdown() override {}
    };

    class entity_panel final : public panel
    {
    public:
        void initialize() override {}
        void update() override;
        void shutdown() override {}

        static entt::entity selected_entity;

    };

    class selected_entity_panel final : public panel
    {
    public:
        void initialize() override {}
        void update() override;
        void shutdown() override {}
    };

    class model_panel final : public panel
    {
    public:
        void initialize() override {}
        void update() override;
        void shutdown() override {}

        static ::orbit::content::model::handle_type selected_model;

    };
}