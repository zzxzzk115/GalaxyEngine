#pragma once

namespace Galaxy
{
    class Application
    {
    public:
        /**
         * @brief Initialize modules of engine
         *
         * @return true: succeed
         * @return false: fail
         */
        bool Init();

        /**
         * @brief Run a main loop of engine
         *
         */
        void Run();

        /**
         * @brief Cleanup
         *
         */
        void Shutdown();

    private:
        bool m_IsRunning = true;
    };
} // namespace Galaxy