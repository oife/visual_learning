#include <vector>
#include <cstdint>

//aim_interfaces::msg::AimInfo

namespace aim_interfaces
{
    namespace msg
    {
        class AimInfo{
        public:
            std::vector<int16_t> coordinate; //机器人坐标系下装甲板的坐标
            int16_t type;          //所识别装甲板的图案类型*
        };
    } // namespace msg
    
} // namespace aim_interfaces
