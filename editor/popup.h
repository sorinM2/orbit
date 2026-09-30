#pragma once
#include <filesystem>

#include "orbit/content/model.h"
#include "panel.h"

namespace editor
{
    class popup
    {
        public:
        popup(const std::string& name) : _name{name} {}
        virtual void initialize() = 0;
        virtual void update() = 0;

        virtual ~popup() {};

    protected:
        std::string _name{};
    };

    class model_popup : public popup
    {
    public:
        model_popup(const std::string& name) : popup(name) {}

        void initialize() override;
        void update() override;

        ~model_popup() override {}
    private:
        file_panel _file_panel;
    };

}
