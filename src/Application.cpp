//
// Created by dwarf on 17/07/2021.
//
#include "../include/Application.hpp"
#include "../include/systems/DataRetriever.hpp"
#include "../include/systems/DataConsoleDisplayer.hpp"
#include "../include/systems/DataExploiter.hpp"
#include "../include/systems/DataGraphicDisplayer.hpp"
Application::Application()
{
    // System order is important !
    systems.add<DataRetriever>();
    systems.add<DataExploiter>();
    systems.add<DataConsoleDisplayer>();
    systems.add<DataGraphicDisplayer>();

    // !!!!!!!!!!!!!!!!!!!!!!!!!!

    systems.configure();
    entityx::Entity e = entities.create();
    e.assign<Data>();

}

void Application::start() {

    std::cout << "Application started !" << std::endl;

    sf::RenderWindow window(sf::VideoMode(640, 480), "Cryptobot");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);
    ImPlot::CreateContext();

    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 5.3f;
        style.FrameRounding = 2.3f;
        style.ScrollbarRounding = 0;
    }

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        sf::Time dt = deltaClock.restart();
        ImGui::SFML::Update(window, dt);

        systems.update<DataRetriever>(dt.asMilliseconds());
        systems.update<DataExploiter>(dt.asMilliseconds());
        systems.update<DataConsoleDisplayer>(dt.asMilliseconds());
        systems.update<DataGraphicDisplayer>(dt.asMilliseconds());

        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

}

