#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <queue>
#include <iostream>
#include <unordered_map>

#define WIDTH 600
#define HEIGHT 600
#define PW 25
#define PH 25
#define FPS 60.0
#define SLEEPTIME 1000.0/FPS
#define SLEEPTIMES SLEEPTIME / 1000.0
#define SPEED 700.0 * SLEEPTIMES

enum ASM {
    NONE = 0x0,
    UP = 0x1,
    DOWN = 0x2,
    LEFT = 0x4,
    RIGHT = 0x8,
    BIRTH = 16,
    DEATH = 32
};


struct Player {
    Player(float px, float py) :
        s(sf::Vector2f(PW, PH))
        {
            s.setPosition(px, py);
        }

    bool exist = true;
    sf::RectangleShape s;
    std::unordered_map<int, ASM> instr;
};

static int ispeed = (int)SPEED;
static void process_instruction(ASM i, Player* lp, bool warping) {

    const sf::Vector2f& vec = lp->s.getPosition();
    int px = vec.x;
    int py = vec.y;

    if (warping) {
        if (i & UP)
            py += ispeed;
        if (i & DOWN)
            py -= ispeed;
        if (i & LEFT)
            px += ispeed;
        if (i & RIGHT)
            px -= ispeed;
        if (i & DEATH)
            lp->exist = true;
        if (i & BIRTH)
            lp->exist = false;
    }
    else {
        if (i & UP)
            py -= ispeed;
        if (i & DOWN)
            py += ispeed;
        if (i & LEFT)
            px -= ispeed;
        if (i & RIGHT)
            px += ispeed;
        if (i & DEATH)
            lp->exist = false;
        if (i & BIRTH)
            lp->exist = true;
    }
    lp->s.setPosition(px, py);
}
int main()
{
    int nframe = 0;
    Player* p = new Player(WIDTH * 0.5, HEIGHT * 0.5);
    p->instr.emplace(nframe, BIRTH);
    std::vector<Player*> all_players;
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Warp");
    sf::Clock Clock;
    bool warping = false;

    all_players.push_back(p);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float ElapsedTime = 0.016;
        //float ElapsedTime = Clock.getElapsedTime().asSeconds();
        Clock.restart();

        std::cout << (float)nframe / FPS << std::endl;

        ASM a = NONE;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (!warping) {
                const sf::Vector2f& vec = p->s.getPosition();
                p->s.setFillColor(sf::Color::Red);
                Player* np = new Player(vec.x, vec.y);
                np->instr.emplace(nframe, BIRTH);
                //np->instr = p->instr;
                p->instr.emplace(nframe, DEATH);
                p = np;
                all_players.push_back(p);
                std::cout << "CLONE" << std::endl;
                warping = true;
            }
        }
        else
            warping = false;

        if (!warping) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
                a |= LEFT;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                a |= RIGHT;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
                a |= UP;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                a |= DOWN;
        }
        if (a != NONE) {
            p->instr.emplace(nframe, a);

            process_instruction(a, p, warping);
        }
        for (auto& lp: all_players)
        {
            if (lp != p) {
                if (lp->instr.count(nframe) > 0) {
                    ASM i = lp->instr[nframe];
                    process_instruction(i, lp, warping);
                }
            }
        }
        window.clear();
        for (auto& lp: all_players) {
            if (lp->exist)
                window.draw(lp->s);
        }
        window.display();
        if (warping) {
            --nframe;
        }
        else
            ++nframe;

        sf::sleep(sf::milliseconds(SLEEPTIME));
    }
    return EXIT_SUCCESS;
}
