#include "../defaultConfigHolder.h"

#ifndef CONFIG_HOLDER_H
#define CONFIG_HOLDER_H

class SetupConfigHolder : public DefaultConfigHolder
{
public:
    static constexpr float getMainEncoderGearRation()
    {
        // replace with gear ratio from motor to output
        return 275.0 / 125904.0;
    }

    static std::unique_ptr<Communication> getCommunicationHandler()
    {
        auto com = std::make_unique<Communication>(&Serial1, 115200);
        com->addCommunicationNode(std::make_unique<DCServoCommunicationHandler>(1));

        return com;
    }
};

// replace with generated output from systemIdent.py
// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// config setps
// x 1) disconnect motor from gearbox
// x 2) compile and transfer to servo nr x
// x 3) open MasterCommunication folder in terminal
// x 4) run 'make'
// x 5) run './executable --servoNr x --recOpticalEncoder --output=opticalEncoderData.txt'
// x 6) run './systemIdent.py --opticalEncoderDataFile=opticalEncoderData.txt'
// x 7) copy past new generated ConfigHolder class, from terminal, over old class
// x 8) compile and transfer to servo nr x with new ConfigHolder class
// x 9) run './executable --servoNr x --recSystemIdentData --output=systemIdentData.txt'
// x 10) run './systemIdent.py --opticalEncoderDataFile=opticalEncoderData.txt --systemIdentDataFile=systemIdentData.txt'
// x 11) copy past new generated ConfigHolder class, from terminal, over old class
// x 12) connect motor to gearbox again
// x 13) compile and transfer to servo nr x with new ConfigHolder class
class ConfigHolder : public SetupConfigHolder
{
public:
    static std::unique_ptr<OpticalEncoderHandler> createMainEncoderHandler()
    {
        std::array<uint16_t, 512> aVec = {3775, 3773, 3772, 3770, 3766, 3763, 3760, 3758, 3755, 3754, 3750, 3746, 3743, 3740, 3738, 3735, 3732, 3730, 3729, 3727, 3725, 3722, 3718, 3714, 3712, 3710, 3705, 3704, 3699, 3697, 3694, 3694, 3690, 3687, 3684, 3680, 3677, 3674, 3671, 3666, 3661, 3659, 3655, 3654, 3647, 3641, 3637, 3632, 3630, 3626, 3619, 3615, 3608, 3603, 3599, 3591, 3589, 3584, 3577, 3576, 3571, 3569, 3563, 3559, 3552, 3545, 3538, 3529, 3523, 3519, 3509, 3505, 3493, 3483, 3475, 3466, 3454, 3464, 3482, 3476, 3430, 3420, 3402, 3385, 3374, 3369, 3351, 3324, 3309, 3292, 3283, 3266, 3247, 3231, 3216, 3197, 3181, 3170, 3156, 3131, 3114, 3109, 3087, 3067, 3046, 3037, 3021, 3015, 3005, 2991, 2980, 2973, 2950, 2929, 2912, 2896, 2881, 2866, 2859, 2846, 2836, 2821, 2804, 2791, 2783, 2763, 2756, 2744, 2734, 2720, 2711, 2697, 2684, 2660, 2645, 2629, 2600, 2570, 2540, 2514, 2507, 2491, 2477, 2468, 2453, 2446, 2416, 2398, 2383, 2355, 2333, 2314, 2290, 2264, 2248, 2224, 2201, 2180, 2163, 2148, 2120, 2103, 2090, 2075, 2051, 2036, 2014, 1999, 1978, 1955, 1933, 1923, 1912, 1900, 1885, 1871, 1859, 1852, 1842, 1825, 1806, 1797, 1781, 1768, 1758, 1743, 1732, 1715, 1691, 1670, 1655, 1646, 1635, 1621, 1612, 1597, 1585, 1573, 1560, 1552, 1554, 1547, 1543, 1542, 1534, 1533, 1546, 1540, 1549, 1537, 1547, 1548, 1551, 1552, 1555, 1564, 1564, 1576, 1578, 1584, 1590, 1599, 1606, 1615, 1624, 1622, 1640, 1648, 1653, 1660, 1672, 1675, 1681, 1690, 1690, 1693, 1699, 1708, 1717, 1723, 1730, 1742, 1749, 1755, 1771, 1778, 1788, 1796, 1812, 1824, 1835, 1842, 1853, 1863, 1866, 1880, 1889, 1900, 1910, 1927, 1942, 1958, 1970, 1982, 1999, 2015, 2028, 2036, 2047, 2059, 2067, 2079, 2093, 2110, 2134, 2143, 2152, 2166, 2186, 2203, 2222, 2237, 2257, 2274, 2289, 2306, 2326, 2341, 2366, 2384, 2407, 2425, 2443, 2454, 2466, 2479, 2491, 2504, 2517, 2528, 2543, 2557, 2569, 2585, 2599, 2614, 2631, 2654, 2663, 2677, 2688, 2701, 2716, 2734, 2749, 2765, 2785, 2803, 2820, 2835, 2853, 2869, 2891, 2903, 2918, 2936, 2951, 2967, 2983, 2998, 3009, 3028, 3045, 3057, 3069, 3079, 3092, 3104, 3114, 3129, 3143, 3156, 3166, 3178, 3193, 3209, 3229, 3247, 3257, 3267, 3288, 3297, 3306, 3318, 3323, 3330, 3340, 3352, 3361, 3370, 3377, 3384, 3390, 3398, 3407, 3416, 3427, 3438, 3449, 3456, 3465, 3469, 3477, 3483, 3491, 3500, 3507, 3515, 3526, 3533, 3538, 3544, 3549, 3555, 3565, 3571, 3571, 3578, 3585, 3591, 3594, 3601, 3606, 3610, 3615, 3620, 3623, 3627, 3632, 3637, 3640, 3644, 3647, 3649, 3653, 3656, 3660, 3663, 3668, 3671, 3674, 3677, 3680, 3683, 3686, 3687, 3689, 3693, 3696, 3692, 3703, 3706, 3710, 3712, 3715, 3715, 3718, 3722, 3725, 3726, 3729, 3731, 3732, 3735, 3738, 3738, 3741, 3743, 3745, 3746, 3749, 3752, 3754, 3756, 3759, 3761, 3763, 3765, 3768, 3771, 3772, 3775, 3776, 3778, 3780, 3783, 3784, 3785, 3787, 3787, 3789, 3787, 3790, 3789, 3789, 3791, 3792, 3793, 3794, 3796, 3796, 3796, 3799, 3799, 3801, 3801, 3802, 3802, 3804, 3804, 3805, 3804, 3806, 3806, 3806, 3807, 3806, 3806, 3807, 3806, 3805, 3805, 3804, 3804, 3804, 3802, 3802, 3800, 3799, 3798, 3796, 3794, 3793, 3792, 3792, 3789, 3788, 3787, 3785, 3785, 3784, 3783};
        std::array<uint16_t, 512> bVec = {3147, 3135, 3124, 3112, 3091, 3064, 3045, 3032, 3018, 3002, 2975, 2954, 2937, 2916, 2900, 2876, 2856, 2843, 2830, 2817, 2802, 2782, 2759, 2732, 2720, 2709, 2689, 2676, 2656, 2638, 2626, 2614, 2596, 2576, 2558, 2534, 2519, 2500, 2486, 2466, 2444, 2434, 2419, 2408, 2390, 2367, 2355, 2342, 2332, 2324, 2305, 2291, 2271, 2256, 2242, 2223, 2214, 2203, 2187, 2178, 2171, 2161, 2148, 2137, 2118, 2107, 2086, 2073, 2064, 2057, 2061, 2051, 2051, 2044, 2040, 2043, 2043, 2051, 2046, 2059, 2046, 2058, 2062, 2059, 2068, 2069, 2072, 2081, 2088, 2092, 2099, 2103, 2109, 2118, 2130, 2135, 2137, 2149, 2148, 2158, 2160, 2169, 2176, 2180, 2188, 2193, 2199, 2194, 2198, 2203, 2205, 2214, 2217, 2225, 2235, 2239, 2250, 2254, 2263, 2269, 2278, 2286, 2298, 2308, 2310, 2321, 2327, 2337, 2341, 2355, 2367, 2376, 2385, 2404, 2416, 2429, 2450, 2475, 2500, 2516, 2524, 2536, 2546, 2554, 2561, 2573, 2591, 2605, 2614, 2636, 2653, 2664, 2683, 2698, 2710, 2727, 2746, 2759, 2773, 2787, 2806, 2817, 2831, 2847, 2863, 2872, 2893, 2906, 2920, 2946, 2962, 2971, 2981, 2990, 3002, 3018, 3027, 3035, 3039, 3053, 3075, 3085, 3102, 3109, 3124, 3132, 3149, 3162, 3177, 3200, 3219, 3232, 3242, 3260, 3270, 3287, 3300, 3320, 3333, 3340, 3348, 3360, 3378, 3389, 3398, 3412, 3406, 3383, 3413, 3441, 3452, 3469, 3479, 3490, 3500, 3514, 3526, 3533, 3544, 3555, 3564, 3571, 3590, 3603, 3613, 3599, 3621, 3636, 3649, 3659, 3663, 3670, 3679, 3685, 3695, 3702, 3706, 3710, 3717, 3726, 3736, 3739, 3742, 3748, 3756, 3763, 3769, 3772, 3777, 3785, 3788, 3790, 3796, 3797, 3800, 3802, 3807, 3807, 3809, 3813, 3817, 3821, 3823, 3826, 3829, 3832, 3833, 3834, 3836, 3839, 3841, 3842, 3843, 3845, 3848, 3851, 3851, 3853, 3857, 3859, 3861, 3863, 3865, 3867, 3869, 3870, 3872, 3873, 3877, 3878, 3880, 3882, 3884, 3885, 3886, 3888, 3889, 3891, 3892, 3892, 3894, 3894, 3896, 3897, 3898, 3898, 3900, 3901, 3901, 3903, 3903, 3904, 3905, 3907, 3908, 3910, 3911, 3913, 3915, 3917, 3919, 3920, 3922, 3923, 3923, 3926, 3927, 3927, 3929, 3929, 3929, 3931, 3931, 3931, 3931, 3931, 3932, 3933, 3932, 3933, 3933, 3934, 3934, 3934, 3935, 3936, 3938, 3939, 3940, 3940, 3942, 3943, 3944, 3944, 3945, 3945, 3946, 3947, 3947, 3947, 3948, 3948, 3948, 3950, 3949, 3949, 3949, 3949, 3949, 3948, 3948, 3948, 3948, 3947, 3946, 3945, 3944, 3943, 3942, 3942, 3941, 3940, 3940, 3939, 3939, 3938, 3936, 3937, 3935, 3934, 3933, 3932, 3931, 3930, 3929, 3928, 3926, 3925, 3923, 3922, 3920, 3920, 3918, 3919, 3916, 3915, 3914, 3913, 3911, 3909, 3909, 3908, 3907, 3905, 3903, 3902, 3902, 3902, 3898, 3898, 3895, 3892, 3890, 3888, 3888, 3886, 3883, 3880, 3876, 3874, 3872, 3869, 3865, 3861, 3859, 3856, 3852, 3847, 3844, 3841, 3837, 3833, 3829, 3826, 3823, 3821, 3818, 3816, 3813, 3813, 3810, 3807, 3806, 3802, 3798, 3794, 3788, 3781, 3774, 3769, 3762, 3757, 3754, 3749, 3742, 3729, 3710, 3699, 3689, 3680, 3669, 3657, 3644, 3632, 3617, 3607, 3603, 3593, 3582, 3570, 3553, 3543, 3530, 3513, 3502, 3491, 3480, 3466, 3440, 3421, 3408, 3403, 3395, 3387, 3377, 3365, 3356, 3345, 3332, 3321, 3304, 3295, 3287, 3276, 3267, 3256, 3249, 3241, 3233, 3224, 3216, 3207};
        return std::make_unique<OpticalEncoderHandler>(aVec, bVec);
    }

    static std::unique_ptr<CurrentController> createCurrentController()
    {
        constexpr float pwmToStallCurrent{2.879142394952704};
        constexpr float backEmfCurrent{-0.0005036711871589321};

        return std::make_unique<CurrentControlModel>(pwmToStallCurrent, backEmfCurrent);
    }

    class ControlParameters : public SetupConfigHolder::DefaultControlParameters
    {
      public:
        //kalman filter observer vector
        static Eigen::Vector3f getKVector()
        {
            Eigen::Vector3f K;
            K << 1.292427046282098,
                430.0061051468128,
                1115.7584415390818;

            return K;
        }

        //system model A matrix
        static Eigen::Matrix3f getAMatrix()
        {
            Eigen::Matrix3f A;
            A << 1.0, 0.0012, 3.59222444222646e-05,
                0.0, 0.9984698247019959, 0.059870407370441005,
                0.0, 0.0, 1.0;

            return A;
        }

        //system model invers A matrix
        static Eigen::Matrix3f getAInvMatrix()
        {
            Eigen::Matrix3f AInv;
            AInv << 1.0, -0.0012018390243872946, 3.6032347561496114e-05,
                0.0, 1.0015325203227456, -0.05996215998646726,
                0.0, 0.0, 1.0;

            return AInv;
        }

        //system model B matrix
        static Eigen::Vector3f getBVector()
        {
            Eigen::Vector3f B;
            B << 3.59222444222646e-05,
                0.059870407370441005,
                0.0;

            return B;
        }

        //state feedback vecktor
        static Eigen::Matrix<float, 4, 1> getLVector(uint8_t controllerSpeed)
        {
            float dt = getAMatrix()(0, 1);
            float a = getAMatrix()(1, 1);
            float b = getBVector()(1);

            return calculateLVector(controllerSpeed, dt, a, b);
        }
    };
};
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#endif
