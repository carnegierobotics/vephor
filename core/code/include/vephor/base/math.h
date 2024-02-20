/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Carnegie Robotics, LLC nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CARNEGIE ROBOTICS, LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <chrono>
#include <Eigen/Core>
#include <manif/manif.h>

namespace vephor
{

using std::vector;
using std::unordered_set;
using std::unordered_map;
using std::array;
using std::min;
using std::max;
using std::string;
using std::to_string;
using std::istream;
using std::ifstream;
using std::ostream;
using std::ofstream;
using std::cout;
using std::endl;
using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::weak_ptr;
using std::make_unique;
using std::chrono::time_point;
using std::chrono::high_resolution_clock;
using std::chrono::nanoseconds;
using std::chrono::duration_cast;
using std::numeric_limits;
using std::pair;
using std::swap;

using Vec2 = Eigen::Matrix<float, 2, 1>;
using Vec2d = Eigen::Matrix<double, 2, 1>;
using Vec3u = Eigen::Matrix<uint8_t, 3, 1>;
using Vec3 = Eigen::Matrix<float, 3, 1>;
using Vec3d = Eigen::Matrix<double, 3, 1>;
using Vec3Ref = Eigen::Ref<const Vec3>;
using Vec4u = Eigen::Matrix<uint8_t, 4, 1>;
using Vec4 = Eigen::Matrix<float, 4, 1>;
using Vec4d = Eigen::Matrix<double, 4, 1>;
using Vec4Ref = Eigen::Ref<const Vec4>;
using VecX = Eigen::Matrix<float, Eigen::Dynamic, 1>;
using VecXRef = Eigen::Ref<const VecX>;
using VecXMap = Eigen::Map<const VecX>;
using VecXui = Eigen::Matrix<uint32_t, Eigen::Dynamic, 1>;

using Vec2i = Eigen::Matrix<int32_t, 2, 1>;
using Vec3i = Eigen::Matrix<int32_t, 3, 1>;

using Mat2 = Eigen::Matrix<float, 2, 2>;
using Mat3 = Eigen::Matrix<float, 3, 3>;
using Mat4 = Eigen::Matrix<float, 4, 4>;
using MatX = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>;
using MatXRef = Eigen::Ref<const MatX>;
using MatXMap = Eigen::Map<const MatX>;

class Color
{
public:
	Color()
	: inner(0,0,0,1)
	{}
	Color(float r, float g, float b, float a=1.0f)
	: inner(r,g,b,a)
	{}

	Color(double r, double g, double b, double a=1.0f)
	: inner(static_cast<float>(r),static_cast<float>(g),static_cast<float>(b),static_cast<float>(a))
	{}

    /*!
     * Construct a color from
     * \param r,g,b,a RGBA color specification with each element in range [0, 255].
     */
    explicit Color(int r, int g, int b, int a=0xFF)
	: inner(static_cast<float>(r) / 255.0f,static_cast<float>(g) / 255.0f,static_cast<float>(b) / 255.0f,
            static_cast<float>(a) / 255.0f)
	{}

    /*!
     * Construct a Color from a hexadecimal RGB or RGBA specification.
     * \param rgba A 32-bit hexadecimal RGB or RGBA color specification (e.g., `0xFF00FF` or `0x0A2CF0F4`,
     *             respectively).
     */
    explicit Color(int rgba)
    {
    	const auto digits = static_cast<int>(std::floor(std::log2(rgba) / 4) + 1);

        const int r = (rgba >> 16) & 0xFF;
        const int g = (rgba >> 8) & 0xFF;
        const int b = rgba & 0xFF;
        const int a = (digits == 8) ? (rgba & 0xFF) : 0xFF;

        inner = Vec4(static_cast<float>(r), static_cast<float>(g), static_cast<float>(b),
                     static_cast<float>(a));
    }

	Color(const Vec3& vec)
	{
		inner.head<3>() = vec;
		inner[3] = 1;
	}
	Color(const Vec3u& vec)
	{
		inner.head<3>() = vec.cast<float>() / 255.0f;
		inner[3] = 1;
	}
	Color(const Vec4& vec)
	: inner(vec)
	{}
	Color(const Vec4u& vec)
	: inner(vec.cast<float>() / 255.0f)
	{}
	const Vec3 getRGB() const
	{
		return inner.head<3>();
	}
	const Vec4& getRGBA() const
	{
		return inner;
	}
	float getAlpha() const
	{
		return inner[3];
	}
private:
	Vec4 inner;
};

class ColorMap
{
public:
    enum class Style
    {
        RAINBOW,
        JET,
        VIRIDIS,
        MAGMA,
        PLASMA,
        INFERNO,
        CIVIDIS
    };

    /*!
     * Construct a color map with a specific style.
     * \param style
     */
    explicit ColorMap(Style style)
    {
        switch (style)
        {
            case Style::RAINBOW:
                colors = {
                        Color(0x00, 0x00, 0xff),
                        Color(0x00, 0xff, 0xff),
                        Color(0x00, 0xff, 0x00),
                        Color(0xff, 0xff, 0x00),
                        Color(0xff, 0x00, 0x00),
                };
                break;
            case Style::JET:
                colors = {
                        Color(0x00, 0x00, 0x0f),
                        Color(0x00, 0x00, 0xff),
                        Color(0x00, 0x0f, 0xff),
                        Color(0x00, 0xff, 0xff),
                        Color(0x0f, 0xff, 0x0f),
                        Color(0xff, 0xff, 0x00),
                        Color(0xff, 0x0f, 0x00),
                        Color(0xff, 0x00, 0x00),
                        Color(0x0f, 0x00, 0x00),
                };
                break;
            case Style::VIRIDIS:
                colors = {
                        Color(0.267004f, 0.004874f, 0.329415f),
                        Color(0.268510f, 0.009605f, 0.335427f),
                        Color(0.269944f, 0.014625f, 0.341379f),
                        Color(0.271305f, 0.019942f, 0.347269f),
                        Color(0.272594f, 0.025563f, 0.353093f),
                        Color(0.273809f, 0.031497f, 0.358853f),
                        Color(0.274952f, 0.037752f, 0.364543f),
                        Color(0.276022f, 0.044167f, 0.370164f),
                        Color(0.277018f, 0.050344f, 0.375715f),
                        Color(0.277941f, 0.056324f, 0.381191f),
                        Color(0.278791f, 0.062145f, 0.386592f),
                        Color(0.279566f, 0.067836f, 0.391917f),
                        Color(0.280267f, 0.073417f, 0.397163f),
                        Color(0.280894f, 0.078907f, 0.402329f),
                        Color(0.281446f, 0.084320f, 0.407414f),
                        Color(0.281924f, 0.089666f, 0.412415f),
                        Color(0.282327f, 0.094955f, 0.417331f),
                        Color(0.282656f, 0.100196f, 0.422160f),
                        Color(0.282910f, 0.105393f, 0.426902f),
                        Color(0.283091f, 0.110553f, 0.431554f),
                        Color(0.283197f, 0.115680f, 0.436115f),
                        Color(0.283229f, 0.120777f, 0.440584f),
                        Color(0.283187f, 0.125848f, 0.444960f),
                        Color(0.283072f, 0.130895f, 0.449241f),
                        Color(0.282884f, 0.135920f, 0.453427f),
                        Color(0.282623f, 0.140926f, 0.457517f),
                        Color(0.282290f, 0.145912f, 0.461510f),
                        Color(0.281887f, 0.150881f, 0.465405f),
                        Color(0.281412f, 0.155834f, 0.469201f),
                        Color(0.280868f, 0.160771f, 0.472899f),
                        Color(0.280255f, 0.165693f, 0.476498f),
                        Color(0.279574f, 0.170599f, 0.479997f),
                        Color(0.278826f, 0.175490f, 0.483397f),
                        Color(0.278012f, 0.180367f, 0.486697f),
                        Color(0.277134f, 0.185228f, 0.489898f),
                        Color(0.276194f, 0.190074f, 0.493001f),
                        Color(0.275191f, 0.194905f, 0.496005f),
                        Color(0.274128f, 0.199721f, 0.498911f),
                        Color(0.273006f, 0.204520f, 0.501721f),
                        Color(0.271828f, 0.209303f, 0.504434f),
                        Color(0.270595f, 0.214069f, 0.507052f),
                        Color(0.269308f, 0.218818f, 0.509577f),
                        Color(0.267968f, 0.223549f, 0.512008f),
                        Color(0.266580f, 0.228262f, 0.514349f),
                        Color(0.265145f, 0.232956f, 0.516599f),
                        Color(0.263663f, 0.237631f, 0.518762f),
                        Color(0.262138f, 0.242286f, 0.520837f),
                        Color(0.260571f, 0.246922f, 0.522828f),
                        Color(0.258965f, 0.251537f, 0.524736f),
                        Color(0.257322f, 0.256130f, 0.526563f),
                        Color(0.255645f, 0.260703f, 0.528312f),
                        Color(0.253935f, 0.265254f, 0.529983f),
                        Color(0.252194f, 0.269783f, 0.531579f),
                        Color(0.250425f, 0.274290f, 0.533103f),
                        Color(0.248629f, 0.278775f, 0.534556f),
                        Color(0.246811f, 0.283237f, 0.535941f),
                        Color(0.244972f, 0.287675f, 0.537260f),
                        Color(0.243113f, 0.292092f, 0.538516f),
                        Color(0.241237f, 0.296485f, 0.539709f),
                        Color(0.239346f, 0.300855f, 0.540844f),
                        Color(0.237441f, 0.305202f, 0.541921f),
                        Color(0.235526f, 0.309527f, 0.542944f),
                        Color(0.233603f, 0.313828f, 0.543914f),
                        Color(0.231674f, 0.318106f, 0.544834f),
                        Color(0.229739f, 0.322361f, 0.545706f),
                        Color(0.227802f, 0.326594f, 0.546532f),
                        Color(0.225863f, 0.330805f, 0.547314f),
                        Color(0.223925f, 0.334994f, 0.548053f),
                        Color(0.221989f, 0.339161f, 0.548752f),
                        Color(0.220057f, 0.343307f, 0.549413f),
                        Color(0.218130f, 0.347432f, 0.550038f),
                        Color(0.216210f, 0.351535f, 0.550627f),
                        Color(0.214298f, 0.355619f, 0.551184f),
                        Color(0.212395f, 0.359683f, 0.551710f),
                        Color(0.210503f, 0.363727f, 0.552206f),
                        Color(0.208623f, 0.367752f, 0.552675f),
                        Color(0.206756f, 0.371758f, 0.553117f),
                        Color(0.204903f, 0.375746f, 0.553533f),
                        Color(0.203063f, 0.379716f, 0.553925f),
                        Color(0.201239f, 0.383670f, 0.554294f),
                        Color(0.199430f, 0.387607f, 0.554642f),
                        Color(0.197636f, 0.391528f, 0.554969f),
                        Color(0.195860f, 0.395433f, 0.555276f),
                        Color(0.194100f, 0.399323f, 0.555565f),
                        Color(0.192357f, 0.403199f, 0.555836f),
                        Color(0.190631f, 0.407061f, 0.556089f),
                        Color(0.188923f, 0.410910f, 0.556326f),
                        Color(0.187231f, 0.414746f, 0.556547f),
                        Color(0.185556f, 0.418570f, 0.556753f),
                        Color(0.183898f, 0.422383f, 0.556944f),
                        Color(0.182256f, 0.426184f, 0.557120f),
                        Color(0.180629f, 0.429975f, 0.557282f),
                        Color(0.179019f, 0.433756f, 0.557430f),
                        Color(0.177423f, 0.437527f, 0.557565f),
                        Color(0.175841f, 0.441290f, 0.557685f),
                        Color(0.174274f, 0.445044f, 0.557792f),
                        Color(0.172719f, 0.448791f, 0.557885f),
                        Color(0.171176f, 0.452530f, 0.557965f),
                        Color(0.169646f, 0.456262f, 0.558030f),
                        Color(0.168126f, 0.459988f, 0.558082f),
                        Color(0.166617f, 0.463708f, 0.558119f),
                        Color(0.165117f, 0.467423f, 0.558141f),
                        Color(0.163625f, 0.471133f, 0.558148f),
                        Color(0.162142f, 0.474838f, 0.558140f),
                        Color(0.160665f, 0.478540f, 0.558115f),
                        Color(0.159194f, 0.482237f, 0.558073f),
                        Color(0.157729f, 0.485932f, 0.558013f),
                        Color(0.156270f, 0.489624f, 0.557936f),
                        Color(0.154815f, 0.493313f, 0.557840f),
                        Color(0.153364f, 0.497000f, 0.557724f),
                        Color(0.151918f, 0.500685f, 0.557587f),
                        Color(0.150476f, 0.504369f, 0.557430f),
                        Color(0.149039f, 0.508051f, 0.557250f),
                        Color(0.147607f, 0.511733f, 0.557049f),
                        Color(0.146180f, 0.515413f, 0.556823f),
                        Color(0.144759f, 0.519093f, 0.556572f),
                        Color(0.143343f, 0.522773f, 0.556295f),
                        Color(0.141935f, 0.526453f, 0.555991f),
                        Color(0.140536f, 0.530132f, 0.555659f),
                        Color(0.139147f, 0.533812f, 0.555298f),
                        Color(0.137770f, 0.537492f, 0.554906f),
                        Color(0.136408f, 0.541173f, 0.554483f),
                        Color(0.135066f, 0.544853f, 0.554029f),
                        Color(0.133743f, 0.548535f, 0.553541f),
                        Color(0.132444f, 0.552216f, 0.553018f),
                        Color(0.131172f, 0.555899f, 0.552459f),
                        Color(0.129933f, 0.559582f, 0.551864f),
                        Color(0.128729f, 0.563265f, 0.551229f),
                        Color(0.127568f, 0.566949f, 0.550556f),
                        Color(0.126453f, 0.570633f, 0.549841f),
                        Color(0.125394f, 0.574318f, 0.549086f),
                        Color(0.124395f, 0.578002f, 0.548287f),
                        Color(0.123463f, 0.581687f, 0.547445f),
                        Color(0.122606f, 0.585371f, 0.546557f),
                        Color(0.121831f, 0.589055f, 0.545623f),
                        Color(0.121148f, 0.592739f, 0.544641f),
                        Color(0.120565f, 0.596422f, 0.543611f),
                        Color(0.120092f, 0.600104f, 0.542530f),
                        Color(0.119738f, 0.603785f, 0.541400f),
                        Color(0.119512f, 0.607464f, 0.540218f),
                        Color(0.119423f, 0.611141f, 0.538982f),
                        Color(0.119483f, 0.614817f, 0.537692f),
                        Color(0.119699f, 0.618490f, 0.536347f),
                        Color(0.120081f, 0.622161f, 0.534946f),
                        Color(0.120638f, 0.625828f, 0.533488f),
                        Color(0.121380f, 0.629492f, 0.531973f),
                        Color(0.122312f, 0.633153f, 0.530398f),
                        Color(0.123444f, 0.636809f, 0.528763f),
                        Color(0.124780f, 0.640461f, 0.527068f),
                        Color(0.126326f, 0.644107f, 0.525311f),
                        Color(0.128087f, 0.647749f, 0.523491f),
                        Color(0.130067f, 0.651384f, 0.521608f),
                        Color(0.132268f, 0.655014f, 0.519661f),
                        Color(0.134692f, 0.658636f, 0.517649f),
                        Color(0.137339f, 0.662252f, 0.515571f),
                        Color(0.140210f, 0.665859f, 0.513427f),
                        Color(0.143303f, 0.669459f, 0.511215f),
                        Color(0.146616f, 0.673050f, 0.508936f),
                        Color(0.150148f, 0.676631f, 0.506589f),
                        Color(0.153894f, 0.680203f, 0.504172f),
                        Color(0.157851f, 0.683765f, 0.501686f),
                        Color(0.162016f, 0.687316f, 0.499129f),
                        Color(0.166383f, 0.690856f, 0.496502f),
                        Color(0.170948f, 0.694384f, 0.493803f),
                        Color(0.175707f, 0.697900f, 0.491033f),
                        Color(0.180653f, 0.701402f, 0.488189f),
                        Color(0.185783f, 0.704891f, 0.485273f),
                        Color(0.191090f, 0.708366f, 0.482284f),
                        Color(0.196571f, 0.711827f, 0.479221f),
                        Color(0.202219f, 0.715272f, 0.476084f),
                        Color(0.208030f, 0.718701f, 0.472873f),
                        Color(0.214000f, 0.722114f, 0.469588f),
                        Color(0.220124f, 0.725509f, 0.466226f),
                        Color(0.226397f, 0.728888f, 0.462789f),
                        Color(0.232815f, 0.732247f, 0.459277f),
                        Color(0.239374f, 0.735588f, 0.455688f),
                        Color(0.246070f, 0.738910f, 0.452024f),
                        Color(0.252899f, 0.742211f, 0.448284f),
                        Color(0.259857f, 0.745492f, 0.444467f),
                        Color(0.266941f, 0.748751f, 0.440573f),
                        Color(0.274149f, 0.751988f, 0.436601f),
                        Color(0.281477f, 0.755203f, 0.432552f),
                        Color(0.288921f, 0.758394f, 0.428426f),
                        Color(0.296479f, 0.761561f, 0.424223f),
                        Color(0.304148f, 0.764704f, 0.419943f),
                        Color(0.311925f, 0.767822f, 0.415586f),
                        Color(0.319809f, 0.770914f, 0.411152f),
                        Color(0.327796f, 0.773980f, 0.406640f),
                        Color(0.335885f, 0.777018f, 0.402049f),
                        Color(0.344074f, 0.780029f, 0.397381f),
                        Color(0.352360f, 0.783011f, 0.392636f),
                        Color(0.360741f, 0.785964f, 0.387814f),
                        Color(0.369214f, 0.788888f, 0.382914f),
                        Color(0.377779f, 0.791781f, 0.377939f),
                        Color(0.386433f, 0.794644f, 0.372886f),
                        Color(0.395174f, 0.797475f, 0.367757f),
                        Color(0.404001f, 0.800275f, 0.362552f),
                        Color(0.412913f, 0.803041f, 0.357269f),
                        Color(0.421908f, 0.805774f, 0.351910f),
                        Color(0.430983f, 0.808473f, 0.346476f),
                        Color(0.440137f, 0.811138f, 0.340967f),
                        Color(0.449368f, 0.813768f, 0.335384f),
                        Color(0.458674f, 0.816363f, 0.329727f),
                        Color(0.468053f, 0.818921f, 0.323998f),
                        Color(0.477504f, 0.821444f, 0.318195f),
                        Color(0.487026f, 0.823929f, 0.312321f),
                        Color(0.496615f, 0.826376f, 0.306377f),
                        Color(0.506271f, 0.828786f, 0.300362f),
                        Color(0.515992f, 0.831158f, 0.294279f),
                        Color(0.525776f, 0.833491f, 0.288127f),
                        Color(0.535621f, 0.835785f, 0.281908f),
                        Color(0.545524f, 0.838039f, 0.275626f),
                        Color(0.555484f, 0.840254f, 0.269281f),
                        Color(0.565498f, 0.842430f, 0.262877f),
                        Color(0.575563f, 0.844566f, 0.256415f),
                        Color(0.585678f, 0.846661f, 0.249897f),
                        Color(0.595839f, 0.848717f, 0.243329f),
                        Color(0.606045f, 0.850733f, 0.236712f),
                        Color(0.616293f, 0.852709f, 0.230052f),
                        Color(0.626579f, 0.854645f, 0.223353f),
                        Color(0.636902f, 0.856542f, 0.216620f),
                        Color(0.647257f, 0.858400f, 0.209861f),
                        Color(0.657642f, 0.860219f, 0.203082f),
                        Color(0.668054f, 0.861999f, 0.196293f),
                        Color(0.678489f, 0.863742f, 0.189503f),
                        Color(0.688944f, 0.865448f, 0.182725f),
                        Color(0.699415f, 0.867117f, 0.175971f),
                        Color(0.709898f, 0.868751f, 0.169257f),
                        Color(0.720391f, 0.870350f, 0.162603f),
                        Color(0.730889f, 0.871916f, 0.156029f),
                        Color(0.741388f, 0.873449f, 0.149561f),
                        Color(0.751884f, 0.874951f, 0.143228f),
                        Color(0.762373f, 0.876424f, 0.137064f),
                        Color(0.772852f, 0.877868f, 0.131109f),
                        Color(0.783315f, 0.879285f, 0.125405f),
                        Color(0.793760f, 0.880678f, 0.120005f),
                        Color(0.804182f, 0.882046f, 0.114965f),
                        Color(0.814576f, 0.883393f, 0.110347f),
                        Color(0.824940f, 0.884720f, 0.106217f),
                        Color(0.835270f, 0.886029f, 0.102646f),
                        Color(0.845561f, 0.887322f, 0.099702f),
                        Color(0.855810f, 0.888601f, 0.097452f),
                        Color(0.866013f, 0.889868f, 0.095953f),
                        Color(0.876168f, 0.891125f, 0.095250f),
                        Color(0.886271f, 0.892374f, 0.095374f),
                        Color(0.896320f, 0.893616f, 0.096335f),
                        Color(0.906311f, 0.894855f, 0.098125f),
                        Color(0.916242f, 0.896091f, 0.100717f),
                        Color(0.926106f, 0.897330f, 0.104071f),
                        Color(0.935904f, 0.898570f, 0.108131f),
                        Color(0.945636f, 0.899815f, 0.112838f),
                        Color(0.955300f, 0.901065f, 0.118128f),
                        Color(0.964894f, 0.902323f, 0.123941f),
                        Color(0.974417f, 0.903590f, 0.130215f),
                        Color(0.983868f, 0.904867f, 0.136897f),
                        Color(0.993248f, 0.906157f, 0.143936f),
                };
                break;
            case Style::MAGMA:
                colors = {
                        Color(0.001462f, 0.000466f, 0.013866f),
                        Color(0.002258f, 0.001295f, 0.018331f),
                        Color(0.003279f, 0.002305f, 0.023708f),
                        Color(0.004512f, 0.003490f, 0.029965f),
                        Color(0.005950f, 0.004843f, 0.037130f),
                        Color(0.007588f, 0.006356f, 0.044973f),
                        Color(0.009426f, 0.008022f, 0.052844f),
                        Color(0.011465f, 0.009828f, 0.060750f),
                        Color(0.013708f, 0.011771f, 0.068667f),
                        Color(0.016156f, 0.013840f, 0.076603f),
                        Color(0.018815f, 0.016026f, 0.084584f),
                        Color(0.021692f, 0.018320f, 0.092610f),
                        Color(0.024792f, 0.020715f, 0.100676f),
                        Color(0.028123f, 0.023201f, 0.108787f),
                        Color(0.031696f, 0.025765f, 0.116965f),
                        Color(0.035520f, 0.028397f, 0.125209f),
                        Color(0.039608f, 0.031090f, 0.133515f),
                        Color(0.043830f, 0.033830f, 0.141886f),
                        Color(0.048062f, 0.036607f, 0.150327f),
                        Color(0.052320f, 0.039407f, 0.158841f),
                        Color(0.056615f, 0.042160f, 0.167446f),
                        Color(0.060949f, 0.044794f, 0.176129f),
                        Color(0.065330f, 0.047318f, 0.184892f),
                        Color(0.069764f, 0.049726f, 0.193735f),
                        Color(0.074257f, 0.052017f, 0.202660f),
                        Color(0.078815f, 0.054184f, 0.211667f),
                        Color(0.083446f, 0.056225f, 0.220755f),
                        Color(0.088155f, 0.058133f, 0.229922f),
                        Color(0.092949f, 0.059904f, 0.239164f),
                        Color(0.097833f, 0.061531f, 0.248477f),
                        Color(0.102815f, 0.063010f, 0.257854f),
                        Color(0.107899f, 0.064335f, 0.267289f),
                        Color(0.113094f, 0.065492f, 0.276784f),
                        Color(0.118405f, 0.066479f, 0.286321f),
                        Color(0.123833f, 0.067295f, 0.295879f),
                        Color(0.129380f, 0.067935f, 0.305443f),
                        Color(0.135053f, 0.068391f, 0.315000f),
                        Color(0.140858f, 0.068654f, 0.324538f),
                        Color(0.146785f, 0.068738f, 0.334011f),
                        Color(0.152839f, 0.068637f, 0.343404f),
                        Color(0.159018f, 0.068354f, 0.352688f),
                        Color(0.165308f, 0.067911f, 0.361816f),
                        Color(0.171713f, 0.067305f, 0.370771f),
                        Color(0.178212f, 0.066576f, 0.379497f),
                        Color(0.184801f, 0.065732f, 0.387973f),
                        Color(0.191460f, 0.064818f, 0.396152f),
                        Color(0.198177f, 0.063862f, 0.404009f),
                        Color(0.204935f, 0.062907f, 0.411514f),
                        Color(0.211718f, 0.061992f, 0.418647f),
                        Color(0.218512f, 0.061158f, 0.425392f),
                        Color(0.225302f, 0.060445f, 0.431742f),
                        Color(0.232077f, 0.059889f, 0.437695f),
                        Color(0.238826f, 0.059517f, 0.443256f),
                        Color(0.245543f, 0.059352f, 0.448436f),
                        Color(0.252220f, 0.059415f, 0.453248f),
                        Color(0.258857f, 0.059706f, 0.457710f),
                        Color(0.265447f, 0.060237f, 0.461840f),
                        Color(0.271994f, 0.060994f, 0.465660f),
                        Color(0.278493f, 0.061978f, 0.469190f),
                        Color(0.284951f, 0.063168f, 0.472451f),
                        Color(0.291366f, 0.064553f, 0.475462f),
                        Color(0.297740f, 0.066117f, 0.478243f),
                        Color(0.304081f, 0.067835f, 0.480812f),
                        Color(0.310382f, 0.069702f, 0.483186f),
                        Color(0.316654f, 0.071690f, 0.485380f),
                        Color(0.322899f, 0.073782f, 0.487408f),
                        Color(0.329114f, 0.075972f, 0.489287f),
                        Color(0.335308f, 0.078236f, 0.491024f),
                        Color(0.341482f, 0.080564f, 0.492631f),
                        Color(0.347636f, 0.082946f, 0.494121f),
                        Color(0.353773f, 0.085373f, 0.495501f),
                        Color(0.359898f, 0.087831f, 0.496778f),
                        Color(0.366012f, 0.090314f, 0.497960f),
                        Color(0.372116f, 0.092816f, 0.499053f),
                        Color(0.378211f, 0.095332f, 0.500067f),
                        Color(0.384299f, 0.097855f, 0.501002f),
                        Color(0.390384f, 0.100379f, 0.501864f),
                        Color(0.396467f, 0.102902f, 0.502658f),
                        Color(0.402548f, 0.105420f, 0.503386f),
                        Color(0.408629f, 0.107930f, 0.504052f),
                        Color(0.414709f, 0.110431f, 0.504662f),
                        Color(0.420791f, 0.112920f, 0.505215f),
                        Color(0.426877f, 0.115395f, 0.505714f),
                        Color(0.432967f, 0.117855f, 0.506160f),
                        Color(0.439062f, 0.120298f, 0.506555f),
                        Color(0.445163f, 0.122724f, 0.506901f),
                        Color(0.451271f, 0.125132f, 0.507198f),
                        Color(0.457386f, 0.127522f, 0.507448f),
                        Color(0.463508f, 0.129893f, 0.507652f),
                        Color(0.469640f, 0.132245f, 0.507809f),
                        Color(0.475780f, 0.134577f, 0.507921f),
                        Color(0.481929f, 0.136891f, 0.507989f),
                        Color(0.488088f, 0.139186f, 0.508011f),
                        Color(0.494258f, 0.141462f, 0.507988f),
                        Color(0.500438f, 0.143719f, 0.507920f),
                        Color(0.506629f, 0.145958f, 0.507806f),
                        Color(0.512831f, 0.148179f, 0.507648f),
                        Color(0.519045f, 0.150383f, 0.507443f),
                        Color(0.525270f, 0.152569f, 0.507192f),
                        Color(0.531507f, 0.154739f, 0.506895f),
                        Color(0.537755f, 0.156894f, 0.506551f),
                        Color(0.544015f, 0.159033f, 0.506159f),
                        Color(0.550287f, 0.161158f, 0.505719f),
                        Color(0.556571f, 0.163269f, 0.505230f),
                        Color(0.562866f, 0.165368f, 0.504692f),
                        Color(0.569172f, 0.167454f, 0.504105f),
                        Color(0.575490f, 0.169530f, 0.503466f),
                        Color(0.581819f, 0.171596f, 0.502777f),
                        Color(0.588158f, 0.173652f, 0.502035f),
                        Color(0.594508f, 0.175701f, 0.501241f),
                        Color(0.600868f, 0.177743f, 0.500394f),
                        Color(0.607238f, 0.179779f, 0.499492f),
                        Color(0.613617f, 0.181811f, 0.498536f),
                        Color(0.620005f, 0.183840f, 0.497524f),
                        Color(0.626401f, 0.185867f, 0.496456f),
                        Color(0.632805f, 0.187893f, 0.495332f),
                        Color(0.639216f, 0.189921f, 0.494150f),
                        Color(0.645633f, 0.191952f, 0.492910f),
                        Color(0.652056f, 0.193986f, 0.491611f),
                        Color(0.658483f, 0.196027f, 0.490253f),
                        Color(0.664915f, 0.198075f, 0.488836f),
                        Color(0.671349f, 0.200133f, 0.487358f),
                        Color(0.677786f, 0.202203f, 0.485819f),
                        Color(0.684224f, 0.204286f, 0.484219f),
                        Color(0.690661f, 0.206384f, 0.482558f),
                        Color(0.697098f, 0.208501f, 0.480835f),
                        Color(0.703532f, 0.210638f, 0.479049f),
                        Color(0.709962f, 0.212797f, 0.477201f),
                        Color(0.716387f, 0.214982f, 0.475290f),
                        Color(0.722805f, 0.217194f, 0.473316f),
                        Color(0.729216f, 0.219437f, 0.471279f),
                        Color(0.735616f, 0.221713f, 0.469180f),
                        Color(0.742004f, 0.224025f, 0.467018f),
                        Color(0.748378f, 0.226377f, 0.464794f),
                        Color(0.754737f, 0.228772f, 0.462509f),
                        Color(0.761077f, 0.231214f, 0.460162f),
                        Color(0.767398f, 0.233705f, 0.457755f),
                        Color(0.773695f, 0.236249f, 0.455289f),
                        Color(0.779968f, 0.238851f, 0.452765f),
                        Color(0.786212f, 0.241514f, 0.450184f),
                        Color(0.792427f, 0.244242f, 0.447543f),
                        Color(0.798608f, 0.247040f, 0.444848f),
                        Color(0.804752f, 0.249911f, 0.442102f),
                        Color(0.810855f, 0.252861f, 0.439305f),
                        Color(0.816914f, 0.255895f, 0.436461f),
                        Color(0.822926f, 0.259016f, 0.433573f),
                        Color(0.828886f, 0.262229f, 0.430644f),
                        Color(0.834791f, 0.265540f, 0.427671f),
                        Color(0.840636f, 0.268953f, 0.424666f),
                        Color(0.846416f, 0.272473f, 0.421631f),
                        Color(0.852126f, 0.276106f, 0.418573f),
                        Color(0.857763f, 0.279857f, 0.415496f),
                        Color(0.863320f, 0.283729f, 0.412403f),
                        Color(0.868793f, 0.287728f, 0.409303f),
                        Color(0.874176f, 0.291859f, 0.406205f),
                        Color(0.879464f, 0.296125f, 0.403118f),
                        Color(0.884651f, 0.300530f, 0.400047f),
                        Color(0.889731f, 0.305079f, 0.397002f),
                        Color(0.894700f, 0.309773f, 0.393995f),
                        Color(0.899552f, 0.314616f, 0.391037f),
                        Color(0.904281f, 0.319610f, 0.388137f),
                        Color(0.908884f, 0.324755f, 0.385308f),
                        Color(0.913354f, 0.330052f, 0.382563f),
                        Color(0.917689f, 0.335500f, 0.379915f),
                        Color(0.921884f, 0.341098f, 0.377376f),
                        Color(0.925937f, 0.346844f, 0.374959f),
                        Color(0.929845f, 0.352734f, 0.372677f),
                        Color(0.933606f, 0.358764f, 0.370541f),
                        Color(0.937221f, 0.364929f, 0.368567f),
                        Color(0.940687f, 0.371224f, 0.366762f),
                        Color(0.944006f, 0.377643f, 0.365136f),
                        Color(0.947180f, 0.384178f, 0.363701f),
                        Color(0.950210f, 0.390820f, 0.362468f),
                        Color(0.953099f, 0.397563f, 0.361438f),
                        Color(0.955849f, 0.404400f, 0.360619f),
                        Color(0.958464f, 0.411324f, 0.360014f),
                        Color(0.960949f, 0.418323f, 0.359630f),
                        Color(0.963310f, 0.425390f, 0.359469f),
                        Color(0.965549f, 0.432519f, 0.359529f),
                        Color(0.967671f, 0.439703f, 0.359810f),
                        Color(0.969680f, 0.446936f, 0.360311f),
                        Color(0.971582f, 0.454210f, 0.361030f),
                        Color(0.973381f, 0.461520f, 0.361965f),
                        Color(0.975082f, 0.468861f, 0.363111f),
                        Color(0.976690f, 0.476226f, 0.364466f),
                        Color(0.978210f, 0.483612f, 0.366025f),
                        Color(0.979645f, 0.491014f, 0.367783f),
                        Color(0.981000f, 0.498428f, 0.369734f),
                        Color(0.982279f, 0.505851f, 0.371874f),
                        Color(0.983485f, 0.513280f, 0.374198f),
                        Color(0.984622f, 0.520713f, 0.376698f),
                        Color(0.985693f, 0.528148f, 0.379371f),
                        Color(0.986700f, 0.535582f, 0.382210f),
                        Color(0.987646f, 0.543015f, 0.385210f),
                        Color(0.988533f, 0.550446f, 0.388365f),
                        Color(0.989363f, 0.557873f, 0.391671f),
                        Color(0.990138f, 0.565296f, 0.395122f),
                        Color(0.990871f, 0.572706f, 0.398714f),
                        Color(0.991558f, 0.580107f, 0.402441f),
                        Color(0.992196f, 0.587502f, 0.406299f),
                        Color(0.992785f, 0.594891f, 0.410283f),
                        Color(0.993326f, 0.602275f, 0.414390f),
                        Color(0.993834f, 0.609644f, 0.418613f),
                        Color(0.994309f, 0.616999f, 0.422950f),
                        Color(0.994738f, 0.624350f, 0.427397f),
                        Color(0.995122f, 0.631696f, 0.431951f),
                        Color(0.995480f, 0.639027f, 0.436607f),
                        Color(0.995810f, 0.646344f, 0.441361f),
                        Color(0.996096f, 0.653659f, 0.446213f),
                        Color(0.996341f, 0.660969f, 0.451160f),
                        Color(0.996580f, 0.668256f, 0.456192f),
                        Color(0.996775f, 0.675541f, 0.461314f),
                        Color(0.996925f, 0.682828f, 0.466526f),
                        Color(0.997077f, 0.690088f, 0.471811f),
                        Color(0.997186f, 0.697349f, 0.477182f),
                        Color(0.997254f, 0.704611f, 0.482635f),
                        Color(0.997325f, 0.711848f, 0.488154f),
                        Color(0.997351f, 0.719089f, 0.493755f),
                        Color(0.997351f, 0.726324f, 0.499428f),
                        Color(0.997341f, 0.733545f, 0.505167f),
                        Color(0.997285f, 0.740772f, 0.510983f),
                        Color(0.997228f, 0.747981f, 0.516859f),
                        Color(0.997138f, 0.755190f, 0.522806f),
                        Color(0.997019f, 0.762398f, 0.528821f),
                        Color(0.996898f, 0.769591f, 0.534892f),
                        Color(0.996727f, 0.776795f, 0.541039f),
                        Color(0.996571f, 0.783977f, 0.547233f),
                        Color(0.996369f, 0.791167f, 0.553499f),
                        Color(0.996162f, 0.798348f, 0.559820f),
                        Color(0.995932f, 0.805527f, 0.566202f),
                        Color(0.995680f, 0.812706f, 0.572645f),
                        Color(0.995424f, 0.819875f, 0.579140f),
                        Color(0.995131f, 0.827052f, 0.585701f),
                        Color(0.994851f, 0.834213f, 0.592307f),
                        Color(0.994524f, 0.841387f, 0.598983f),
                        Color(0.994222f, 0.848540f, 0.605696f),
                        Color(0.993866f, 0.855711f, 0.612482f),
                        Color(0.993545f, 0.862859f, 0.619299f),
                        Color(0.993170f, 0.870024f, 0.626189f),
                        Color(0.992831f, 0.877168f, 0.633109f),
                        Color(0.992440f, 0.884330f, 0.640099f),
                        Color(0.992089f, 0.891470f, 0.647116f),
                        Color(0.991688f, 0.898627f, 0.654202f),
                        Color(0.991332f, 0.905763f, 0.661309f),
                        Color(0.990930f, 0.912915f, 0.668481f),
                        Color(0.990570f, 0.920049f, 0.675675f),
                        Color(0.990175f, 0.927196f, 0.682926f),
                        Color(0.989815f, 0.934329f, 0.690198f),
                        Color(0.989434f, 0.941470f, 0.697519f),
                        Color(0.989077f, 0.948604f, 0.704863f),
                        Color(0.988717f, 0.955742f, 0.712242f),
                        Color(0.988367f, 0.962878f, 0.719649f),
                        Color(0.988033f, 0.970012f, 0.727077f),
                        Color(0.987691f, 0.977154f, 0.734536f),
                        Color(0.987387f, 0.984288f, 0.742002f),
                        Color(0.987053f, 0.991438f, 0.749504f),
                };
                break;
            case Style::PLASMA:
                colors = {
                        Color(0.050383f, 0.029803f, 0.527975f),
                        Color(0.063536f, 0.028426f, 0.533124f),
                        Color(0.075353f, 0.027206f, 0.538007f),
                        Color(0.086222f, 0.026125f, 0.542658f),
                        Color(0.096379f, 0.025165f, 0.547103f),
                        Color(0.105980f, 0.024309f, 0.551368f),
                        Color(0.115124f, 0.023556f, 0.555468f),
                        Color(0.123903f, 0.022878f, 0.559423f),
                        Color(0.132381f, 0.022258f, 0.563250f),
                        Color(0.140603f, 0.021687f, 0.566959f),
                        Color(0.148607f, 0.021154f, 0.570562f),
                        Color(0.156421f, 0.020651f, 0.574065f),
                        Color(0.164070f, 0.020171f, 0.577478f),
                        Color(0.171574f, 0.019706f, 0.580806f),
                        Color(0.178950f, 0.019252f, 0.584054f),
                        Color(0.186213f, 0.018803f, 0.587228f),
                        Color(0.193374f, 0.018354f, 0.590330f),
                        Color(0.200445f, 0.017902f, 0.593364f),
                        Color(0.207435f, 0.017442f, 0.596333f),
                        Color(0.214350f, 0.016973f, 0.599239f),
                        Color(0.221197f, 0.016497f, 0.602083f),
                        Color(0.227983f, 0.016007f, 0.604867f),
                        Color(0.234715f, 0.015502f, 0.607592f),
                        Color(0.241396f, 0.014979f, 0.610259f),
                        Color(0.248032f, 0.014439f, 0.612868f),
                        Color(0.254627f, 0.013882f, 0.615419f),
                        Color(0.261183f, 0.013308f, 0.617911f),
                        Color(0.267703f, 0.012716f, 0.620346f),
                        Color(0.274191f, 0.012109f, 0.622722f),
                        Color(0.280648f, 0.011488f, 0.625038f),
                        Color(0.287076f, 0.010855f, 0.627295f),
                        Color(0.293478f, 0.010213f, 0.629490f),
                        Color(0.299855f, 0.009561f, 0.631624f),
                        Color(0.306210f, 0.008902f, 0.633694f),
                        Color(0.312543f, 0.008239f, 0.635700f),
                        Color(0.318856f, 0.007576f, 0.637640f),
                        Color(0.325150f, 0.006915f, 0.639512f),
                        Color(0.331426f, 0.006261f, 0.641316f),
                        Color(0.337683f, 0.005618f, 0.643049f),
                        Color(0.343925f, 0.004991f, 0.644710f),
                        Color(0.350150f, 0.004382f, 0.646298f),
                        Color(0.356359f, 0.003798f, 0.647810f),
                        Color(0.362553f, 0.003243f, 0.649245f),
                        Color(0.368733f, 0.002724f, 0.650601f),
                        Color(0.374897f, 0.002245f, 0.651876f),
                        Color(0.381047f, 0.001814f, 0.653068f),
                        Color(0.387183f, 0.001434f, 0.654177f),
                        Color(0.393304f, 0.001114f, 0.655199f),
                        Color(0.399411f, 0.000859f, 0.656133f),
                        Color(0.405503f, 0.000678f, 0.656977f),
                        Color(0.411580f, 0.000577f, 0.657730f),
                        Color(0.417642f, 0.000564f, 0.658390f),
                        Color(0.423689f, 0.000646f, 0.658956f),
                        Color(0.429719f, 0.000831f, 0.659425f),
                        Color(0.435734f, 0.001127f, 0.659797f),
                        Color(0.441732f, 0.001540f, 0.660069f),
                        Color(0.447714f, 0.002080f, 0.660240f),
                        Color(0.453677f, 0.002755f, 0.660310f),
                        Color(0.459623f, 0.003574f, 0.660277f),
                        Color(0.465550f, 0.004545f, 0.660139f),
                        Color(0.471457f, 0.005678f, 0.659897f),
                        Color(0.477344f, 0.006980f, 0.659549f),
                        Color(0.483210f, 0.008460f, 0.659095f),
                        Color(0.489055f, 0.010127f, 0.658534f),
                        Color(0.494877f, 0.011990f, 0.657865f),
                        Color(0.500678f, 0.014055f, 0.657088f),
                        Color(0.506454f, 0.016333f, 0.656202f),
                        Color(0.512206f, 0.018833f, 0.655209f),
                        Color(0.517933f, 0.021563f, 0.654109f),
                        Color(0.523633f, 0.024532f, 0.652901f),
                        Color(0.529306f, 0.027747f, 0.651586f),
                        Color(0.534952f, 0.031217f, 0.650165f),
                        Color(0.540570f, 0.034950f, 0.648640f),
                        Color(0.546157f, 0.038954f, 0.647010f),
                        Color(0.551715f, 0.043136f, 0.645277f),
                        Color(0.557243f, 0.047331f, 0.643443f),
                        Color(0.562738f, 0.051545f, 0.641509f),
                        Color(0.568201f, 0.055778f, 0.639477f),
                        Color(0.573632f, 0.060028f, 0.637349f),
                        Color(0.579029f, 0.064296f, 0.635126f),
                        Color(0.584391f, 0.068579f, 0.632812f),
                        Color(0.589719f, 0.072878f, 0.630408f),
                        Color(0.595011f, 0.077190f, 0.627917f),
                        Color(0.600266f, 0.081516f, 0.625342f),
                        Color(0.605485f, 0.085854f, 0.622686f),
                        Color(0.610667f, 0.090204f, 0.619951f),
                        Color(0.615812f, 0.094564f, 0.617140f),
                        Color(0.620919f, 0.098934f, 0.614257f),
                        Color(0.625987f, 0.103312f, 0.611305f),
                        Color(0.631017f, 0.107699f, 0.608287f),
                        Color(0.636008f, 0.112092f, 0.605205f),
                        Color(0.640959f, 0.116492f, 0.602065f),
                        Color(0.645872f, 0.120898f, 0.598867f),
                        Color(0.650746f, 0.125309f, 0.595617f),
                        Color(0.655580f, 0.129725f, 0.592317f),
                        Color(0.660374f, 0.134144f, 0.588971f),
                        Color(0.665129f, 0.138566f, 0.585582f),
                        Color(0.669845f, 0.142992f, 0.582154f),
                        Color(0.674522f, 0.147419f, 0.578688f),
                        Color(0.679160f, 0.151848f, 0.575189f),
                        Color(0.683758f, 0.156278f, 0.571660f),
                        Color(0.688318f, 0.160709f, 0.568103f),
                        Color(0.692840f, 0.165141f, 0.564522f),
                        Color(0.697324f, 0.169573f, 0.560919f),
                        Color(0.701769f, 0.174005f, 0.557296f),
                        Color(0.706178f, 0.178437f, 0.553657f),
                        Color(0.710549f, 0.182868f, 0.550004f),
                        Color(0.714883f, 0.187299f, 0.546338f),
                        Color(0.719181f, 0.191729f, 0.542663f),
                        Color(0.723444f, 0.196158f, 0.538981f),
                        Color(0.727670f, 0.200586f, 0.535293f),
                        Color(0.731862f, 0.205013f, 0.531601f),
                        Color(0.736019f, 0.209439f, 0.527908f),
                        Color(0.740143f, 0.213864f, 0.524216f),
                        Color(0.744232f, 0.218288f, 0.520524f),
                        Color(0.748289f, 0.222711f, 0.516834f),
                        Color(0.752312f, 0.227133f, 0.513149f),
                        Color(0.756304f, 0.231555f, 0.509468f),
                        Color(0.760264f, 0.235976f, 0.505794f),
                        Color(0.764193f, 0.240396f, 0.502126f),
                        Color(0.768090f, 0.244817f, 0.498465f),
                        Color(0.771958f, 0.249237f, 0.494813f),
                        Color(0.775796f, 0.253658f, 0.491171f),
                        Color(0.779604f, 0.258078f, 0.487539f),
                        Color(0.783383f, 0.262500f, 0.483918f),
                        Color(0.787133f, 0.266922f, 0.480307f),
                        Color(0.790855f, 0.271345f, 0.476706f),
                        Color(0.794549f, 0.275770f, 0.473117f),
                        Color(0.798216f, 0.280197f, 0.469538f),
                        Color(0.801855f, 0.284626f, 0.465971f),
                        Color(0.805467f, 0.289057f, 0.462415f),
                        Color(0.809052f, 0.293491f, 0.458870f),
                        Color(0.812612f, 0.297928f, 0.455338f),
                        Color(0.816144f, 0.302368f, 0.451816f),
                        Color(0.819651f, 0.306812f, 0.448306f),
                        Color(0.823132f, 0.311261f, 0.444806f),
                        Color(0.826588f, 0.315714f, 0.441316f),
                        Color(0.830018f, 0.320172f, 0.437836f),
                        Color(0.833422f, 0.324635f, 0.434366f),
                        Color(0.836801f, 0.329105f, 0.430905f),
                        Color(0.840155f, 0.333580f, 0.427455f),
                        Color(0.843484f, 0.338062f, 0.424013f),
                        Color(0.846788f, 0.342551f, 0.420579f),
                        Color(0.850066f, 0.347048f, 0.417153f),
                        Color(0.853319f, 0.351553f, 0.413734f),
                        Color(0.856547f, 0.356066f, 0.410322f),
                        Color(0.859750f, 0.360588f, 0.406917f),
                        Color(0.862927f, 0.365119f, 0.403519f),
                        Color(0.866078f, 0.369660f, 0.400126f),
                        Color(0.869203f, 0.374212f, 0.396738f),
                        Color(0.872303f, 0.378774f, 0.393355f),
                        Color(0.875376f, 0.383347f, 0.389976f),
                        Color(0.878423f, 0.387932f, 0.386600f),
                        Color(0.881443f, 0.392529f, 0.383229f),
                        Color(0.884436f, 0.397139f, 0.379860f),
                        Color(0.887402f, 0.401762f, 0.376494f),
                        Color(0.890340f, 0.406398f, 0.373130f),
                        Color(0.893250f, 0.411048f, 0.369768f),
                        Color(0.896131f, 0.415712f, 0.366407f),
                        Color(0.898984f, 0.420392f, 0.363047f),
                        Color(0.901807f, 0.425087f, 0.359688f),
                        Color(0.904601f, 0.429797f, 0.356329f),
                        Color(0.907365f, 0.434524f, 0.352970f),
                        Color(0.910098f, 0.439268f, 0.349610f),
                        Color(0.912800f, 0.444029f, 0.346251f),
                        Color(0.915471f, 0.448807f, 0.342890f),
                        Color(0.918109f, 0.453603f, 0.339529f),
                        Color(0.920714f, 0.458417f, 0.336166f),
                        Color(0.923287f, 0.463251f, 0.332801f),
                        Color(0.925825f, 0.468103f, 0.329435f),
                        Color(0.928329f, 0.472975f, 0.326067f),
                        Color(0.930798f, 0.477867f, 0.322697f),
                        Color(0.933232f, 0.482780f, 0.319325f),
                        Color(0.935630f, 0.487712f, 0.315952f),
                        Color(0.937990f, 0.492667f, 0.312575f),
                        Color(0.940313f, 0.497642f, 0.309197f),
                        Color(0.942598f, 0.502639f, 0.305816f),
                        Color(0.944844f, 0.507658f, 0.302433f),
                        Color(0.947051f, 0.512699f, 0.299049f),
                        Color(0.949217f, 0.517763f, 0.295662f),
                        Color(0.951344f, 0.522850f, 0.292275f),
                        Color(0.953428f, 0.527960f, 0.288883f),
                        Color(0.955470f, 0.533093f, 0.285490f),
                        Color(0.957469f, 0.538250f, 0.282096f),
                        Color(0.959424f, 0.543431f, 0.278701f),
                        Color(0.961336f, 0.548636f, 0.275305f),
                        Color(0.963203f, 0.553865f, 0.271909f),
                        Color(0.965024f, 0.559118f, 0.268513f),
                        Color(0.966798f, 0.564396f, 0.265118f),
                        Color(0.968526f, 0.569700f, 0.261721f),
                        Color(0.970205f, 0.575028f, 0.258325f),
                        Color(0.971835f, 0.580382f, 0.254931f),
                        Color(0.973416f, 0.585761f, 0.251540f),
                        Color(0.974947f, 0.591165f, 0.248151f),
                        Color(0.976428f, 0.596595f, 0.244767f),
                        Color(0.977856f, 0.602051f, 0.241387f),
                        Color(0.979233f, 0.607532f, 0.238013f),
                        Color(0.980556f, 0.613039f, 0.234646f),
                        Color(0.981826f, 0.618572f, 0.231287f),
                        Color(0.983041f, 0.624131f, 0.227937f),
                        Color(0.984199f, 0.629718f, 0.224595f),
                        Color(0.985301f, 0.635330f, 0.221265f),
                        Color(0.986345f, 0.640969f, 0.217948f),
                        Color(0.987332f, 0.646633f, 0.214648f),
                        Color(0.988260f, 0.652325f, 0.211364f),
                        Color(0.989128f, 0.658043f, 0.208100f),
                        Color(0.989935f, 0.663787f, 0.204859f),
                        Color(0.990681f, 0.669558f, 0.201642f),
                        Color(0.991365f, 0.675355f, 0.198453f),
                        Color(0.991985f, 0.681179f, 0.195295f),
                        Color(0.992541f, 0.687030f, 0.192170f),
                        Color(0.993032f, 0.692907f, 0.189084f),
                        Color(0.993456f, 0.698810f, 0.186041f),
                        Color(0.993814f, 0.704741f, 0.183043f),
                        Color(0.994103f, 0.710698f, 0.180097f),
                        Color(0.994324f, 0.716681f, 0.177208f),
                        Color(0.994474f, 0.722691f, 0.174381f),
                        Color(0.994553f, 0.728728f, 0.171622f),
                        Color(0.994561f, 0.734791f, 0.168938f),
                        Color(0.994495f, 0.740880f, 0.166335f),
                        Color(0.994355f, 0.746995f, 0.163821f),
                        Color(0.994141f, 0.753137f, 0.161404f),
                        Color(0.993851f, 0.759304f, 0.159092f),
                        Color(0.993482f, 0.765499f, 0.156891f),
                        Color(0.993033f, 0.771720f, 0.154808f),
                        Color(0.992505f, 0.777967f, 0.152855f),
                        Color(0.991897f, 0.784239f, 0.151042f),
                        Color(0.991209f, 0.790537f, 0.149377f),
                        Color(0.990439f, 0.796859f, 0.147870f),
                        Color(0.989587f, 0.803205f, 0.146529f),
                        Color(0.988648f, 0.809579f, 0.145357f),
                        Color(0.987621f, 0.815978f, 0.144363f),
                        Color(0.986509f, 0.822401f, 0.143557f),
                        Color(0.985314f, 0.828846f, 0.142945f),
                        Color(0.984031f, 0.835315f, 0.142528f),
                        Color(0.982653f, 0.841812f, 0.142303f),
                        Color(0.981190f, 0.848329f, 0.142279f),
                        Color(0.979644f, 0.854866f, 0.142453f),
                        Color(0.977995f, 0.861432f, 0.142808f),
                        Color(0.976265f, 0.868016f, 0.143351f),
                        Color(0.974443f, 0.874622f, 0.144061f),
                        Color(0.972530f, 0.881250f, 0.144923f),
                        Color(0.970533f, 0.887896f, 0.145919f),
                        Color(0.968443f, 0.894564f, 0.147014f),
                        Color(0.966271f, 0.901249f, 0.148180f),
                        Color(0.964021f, 0.907950f, 0.149370f),
                        Color(0.961681f, 0.914672f, 0.150520f),
                        Color(0.959276f, 0.921407f, 0.151566f),
                        Color(0.956808f, 0.928152f, 0.152409f),
                        Color(0.954287f, 0.934908f, 0.152921f),
                        Color(0.951726f, 0.941671f, 0.152925f),
                        Color(0.949151f, 0.948435f, 0.152178f),
                        Color(0.946602f, 0.955190f, 0.150328f),
                        Color(0.944152f, 0.961916f, 0.146861f),
                        Color(0.941896f, 0.968590f, 0.140956f),
                        Color(0.940015f, 0.975158f, 0.131326f),
                };
                break;
            case Style::INFERNO:
                colors = {
                        Color(0.001462f, 0.000466f, 0.013866f),
                        Color(0.002267f, 0.001270f, 0.018570f),
                        Color(0.003299f, 0.002249f, 0.024239f),
                        Color(0.004547f, 0.003392f, 0.030909f),
                        Color(0.006006f, 0.004692f, 0.038558f),
                        Color(0.007676f, 0.006136f, 0.046836f),
                        Color(0.009561f, 0.007713f, 0.055143f),
                        Color(0.011663f, 0.009417f, 0.063460f),
                        Color(0.013995f, 0.011225f, 0.071862f),
                        Color(0.016561f, 0.013136f, 0.080282f),
                        Color(0.019373f, 0.015133f, 0.088767f),
                        Color(0.022447f, 0.017199f, 0.097327f),
                        Color(0.025793f, 0.019331f, 0.105930f),
                        Color(0.029432f, 0.021503f, 0.114621f),
                        Color(0.033385f, 0.023702f, 0.123397f),
                        Color(0.037668f, 0.025921f, 0.132232f),
                        Color(0.042253f, 0.028139f, 0.141141f),
                        Color(0.046915f, 0.030324f, 0.150164f),
                        Color(0.051644f, 0.032474f, 0.159254f),
                        Color(0.056449f, 0.034569f, 0.168414f),
                        Color(0.061340f, 0.036590f, 0.177642f),
                        Color(0.066331f, 0.038504f, 0.186962f),
                        Color(0.071429f, 0.040294f, 0.196354f),
                        Color(0.076637f, 0.041905f, 0.205799f),
                        Color(0.081962f, 0.043328f, 0.215289f),
                        Color(0.087411f, 0.044556f, 0.224813f),
                        Color(0.092990f, 0.045583f, 0.234358f),
                        Color(0.098702f, 0.046402f, 0.243904f),
                        Color(0.104551f, 0.047008f, 0.253430f),
                        Color(0.110536f, 0.047399f, 0.262912f),
                        Color(0.116656f, 0.047574f, 0.272321f),
                        Color(0.122908f, 0.047536f, 0.281624f),
                        Color(0.129285f, 0.047293f, 0.290788f),
                        Color(0.135778f, 0.046856f, 0.299776f),
                        Color(0.142378f, 0.046242f, 0.308553f),
                        Color(0.149073f, 0.045468f, 0.317085f),
                        Color(0.155850f, 0.044559f, 0.325338f),
                        Color(0.162689f, 0.043554f, 0.333277f),
                        Color(0.169575f, 0.042489f, 0.340874f),
                        Color(0.176493f, 0.041402f, 0.348111f),
                        Color(0.183429f, 0.040329f, 0.354971f),
                        Color(0.190367f, 0.039309f, 0.361447f),
                        Color(0.197297f, 0.038400f, 0.367535f),
                        Color(0.204209f, 0.037632f, 0.373238f),
                        Color(0.211095f, 0.037030f, 0.378563f),
                        Color(0.217949f, 0.036615f, 0.383522f),
                        Color(0.224763f, 0.036405f, 0.388129f),
                        Color(0.231538f, 0.036405f, 0.392400f),
                        Color(0.238273f, 0.036621f, 0.396353f),
                        Color(0.244967f, 0.037055f, 0.400007f),
                        Color(0.251620f, 0.037705f, 0.403378f),
                        Color(0.258234f, 0.038571f, 0.406485f),
                        Color(0.264810f, 0.039647f, 0.409345f),
                        Color(0.271347f, 0.040922f, 0.411976f),
                        Color(0.277850f, 0.042353f, 0.414392f),
                        Color(0.284321f, 0.043933f, 0.416608f),
                        Color(0.290763f, 0.045644f, 0.418637f),
                        Color(0.297178f, 0.047470f, 0.420491f),
                        Color(0.303568f, 0.049396f, 0.422182f),
                        Color(0.309935f, 0.051407f, 0.423721f),
                        Color(0.316282f, 0.053490f, 0.425116f),
                        Color(0.322610f, 0.055634f, 0.426377f),
                        Color(0.328921f, 0.057827f, 0.427511f),
                        Color(0.335217f, 0.060060f, 0.428524f),
                        Color(0.341500f, 0.062325f, 0.429425f),
                        Color(0.347771f, 0.064616f, 0.430217f),
                        Color(0.354032f, 0.066925f, 0.430906f),
                        Color(0.360284f, 0.069247f, 0.431497f),
                        Color(0.366529f, 0.071579f, 0.431994f),
                        Color(0.372768f, 0.073915f, 0.432400f),
                        Color(0.379001f, 0.076253f, 0.432719f),
                        Color(0.385228f, 0.078591f, 0.432955f),
                        Color(0.391453f, 0.080927f, 0.433109f),
                        Color(0.397674f, 0.083257f, 0.433183f),
                        Color(0.403894f, 0.085580f, 0.433179f),
                        Color(0.410113f, 0.087896f, 0.433098f),
                        Color(0.416331f, 0.090203f, 0.432943f),
                        Color(0.422549f, 0.092501f, 0.432714f),
                        Color(0.428768f, 0.094790f, 0.432412f),
                        Color(0.434987f, 0.097069f, 0.432039f),
                        Color(0.441207f, 0.099338f, 0.431594f),
                        Color(0.447428f, 0.101597f, 0.431080f),
                        Color(0.453651f, 0.103848f, 0.430498f),
                        Color(0.459875f, 0.106089f, 0.429846f),
                        Color(0.466100f, 0.108322f, 0.429125f),
                        Color(0.472328f, 0.110547f, 0.428334f),
                        Color(0.478558f, 0.112764f, 0.427475f),
                        Color(0.484789f, 0.114974f, 0.426548f),
                        Color(0.491022f, 0.117179f, 0.425552f),
                        Color(0.497257f, 0.119379f, 0.424488f),
                        Color(0.503493f, 0.121575f, 0.423356f),
                        Color(0.509730f, 0.123769f, 0.422156f),
                        Color(0.515967f, 0.125960f, 0.420887f),
                        Color(0.522206f, 0.128150f, 0.419549f),
                        Color(0.528444f, 0.130341f, 0.418142f),
                        Color(0.534683f, 0.132534f, 0.416667f),
                        Color(0.540920f, 0.134729f, 0.415123f),
                        Color(0.547157f, 0.136929f, 0.413511f),
                        Color(0.553392f, 0.139134f, 0.411829f),
                        Color(0.559624f, 0.141346f, 0.410078f),
                        Color(0.565854f, 0.143567f, 0.408258f),
                        Color(0.572081f, 0.145797f, 0.406369f),
                        Color(0.578304f, 0.148039f, 0.404411f),
                        Color(0.584521f, 0.150294f, 0.402385f),
                        Color(0.590734f, 0.152563f, 0.400290f),
                        Color(0.596940f, 0.154848f, 0.398125f),
                        Color(0.603139f, 0.157151f, 0.395891f),
                        Color(0.609330f, 0.159474f, 0.393589f),
                        Color(0.615513f, 0.161817f, 0.391219f),
                        Color(0.621685f, 0.164184f, 0.388781f),
                        Color(0.627847f, 0.166575f, 0.386276f),
                        Color(0.633998f, 0.168992f, 0.383704f),
                        Color(0.640135f, 0.171438f, 0.381065f),
                        Color(0.646260f, 0.173914f, 0.378359f),
                        Color(0.652369f, 0.176421f, 0.375586f),
                        Color(0.658463f, 0.178962f, 0.372748f),
                        Color(0.664540f, 0.181539f, 0.369846f),
                        Color(0.670599f, 0.184153f, 0.366879f),
                        Color(0.676638f, 0.186807f, 0.363849f),
                        Color(0.682656f, 0.189501f, 0.360757f),
                        Color(0.688653f, 0.192239f, 0.357603f),
                        Color(0.694627f, 0.195021f, 0.354388f),
                        Color(0.700576f, 0.197851f, 0.351113f),
                        Color(0.706500f, 0.200728f, 0.347777f),
                        Color(0.712396f, 0.203656f, 0.344383f),
                        Color(0.718264f, 0.206636f, 0.340931f),
                        Color(0.724103f, 0.209670f, 0.337424f),
                        Color(0.729909f, 0.212759f, 0.333861f),
                        Color(0.735683f, 0.215906f, 0.330245f),
                        Color(0.741423f, 0.219112f, 0.326576f),
                        Color(0.747127f, 0.222378f, 0.322856f),
                        Color(0.752794f, 0.225706f, 0.319085f),
                        Color(0.758422f, 0.229097f, 0.315266f),
                        Color(0.764010f, 0.232554f, 0.311399f),
                        Color(0.769556f, 0.236077f, 0.307485f),
                        Color(0.775059f, 0.239667f, 0.303526f),
                        Color(0.780517f, 0.243327f, 0.299523f),
                        Color(0.785929f, 0.247056f, 0.295477f),
                        Color(0.791293f, 0.250856f, 0.291390f),
                        Color(0.796607f, 0.254728f, 0.287264f),
                        Color(0.801871f, 0.258674f, 0.283099f),
                        Color(0.807082f, 0.262692f, 0.278898f),
                        Color(0.812239f, 0.266786f, 0.274661f),
                        Color(0.817341f, 0.270954f, 0.270390f),
                        Color(0.822386f, 0.275197f, 0.266085f),
                        Color(0.827372f, 0.279517f, 0.261750f),
                        Color(0.832299f, 0.283913f, 0.257383f),
                        Color(0.837165f, 0.288385f, 0.252988f),
                        Color(0.841969f, 0.292933f, 0.248564f),
                        Color(0.846709f, 0.297559f, 0.244113f),
                        Color(0.851384f, 0.302260f, 0.239636f),
                        Color(0.855992f, 0.307038f, 0.235133f),
                        Color(0.860533f, 0.311892f, 0.230606f),
                        Color(0.865006f, 0.316822f, 0.226055f),
                        Color(0.869409f, 0.321827f, 0.221482f),
                        Color(0.873741f, 0.326906f, 0.216886f),
                        Color(0.878001f, 0.332060f, 0.212268f),
                        Color(0.882188f, 0.337287f, 0.207628f),
                        Color(0.886302f, 0.342586f, 0.202968f),
                        Color(0.890341f, 0.347957f, 0.198286f),
                        Color(0.894305f, 0.353399f, 0.193584f),
                        Color(0.898192f, 0.358911f, 0.188860f),
                        Color(0.902003f, 0.364492f, 0.184116f),
                        Color(0.905735f, 0.370140f, 0.179350f),
                        Color(0.909390f, 0.375856f, 0.174563f),
                        Color(0.912966f, 0.381636f, 0.169755f),
                        Color(0.916462f, 0.387481f, 0.164924f),
                        Color(0.919879f, 0.393389f, 0.160070f),
                        Color(0.923215f, 0.399359f, 0.155193f),
                        Color(0.926470f, 0.405389f, 0.150292f),
                        Color(0.929644f, 0.411479f, 0.145367f),
                        Color(0.932737f, 0.417627f, 0.140417f),
                        Color(0.935747f, 0.423831f, 0.135440f),
                        Color(0.938675f, 0.430091f, 0.130438f),
                        Color(0.941521f, 0.436405f, 0.125409f),
                        Color(0.944285f, 0.442772f, 0.120354f),
                        Color(0.946965f, 0.449191f, 0.115272f),
                        Color(0.949562f, 0.455660f, 0.110164f),
                        Color(0.952075f, 0.462178f, 0.105031f),
                        Color(0.954506f, 0.468744f, 0.099874f),
                        Color(0.956852f, 0.475356f, 0.094695f),
                        Color(0.959114f, 0.482014f, 0.089499f),
                        Color(0.961293f, 0.488716f, 0.084289f),
                        Color(0.963387f, 0.495462f, 0.079073f),
                        Color(0.965397f, 0.502249f, 0.073859f),
                        Color(0.967322f, 0.509078f, 0.068659f),
                        Color(0.969163f, 0.515946f, 0.063488f),
                        Color(0.970919f, 0.522853f, 0.058367f),
                        Color(0.972590f, 0.529798f, 0.053324f),
                        Color(0.974176f, 0.536780f, 0.048392f),
                        Color(0.975677f, 0.543798f, 0.043618f),
                        Color(0.977092f, 0.550850f, 0.039050f),
                        Color(0.978422f, 0.557937f, 0.034931f),
                        Color(0.979666f, 0.565057f, 0.031409f),
                        Color(0.980824f, 0.572209f, 0.028508f),
                        Color(0.981895f, 0.579392f, 0.026250f),
                        Color(0.982881f, 0.586606f, 0.024661f),
                        Color(0.983779f, 0.593849f, 0.023770f),
                        Color(0.984591f, 0.601122f, 0.023606f),
                        Color(0.985315f, 0.608422f, 0.024202f),
                        Color(0.985952f, 0.615750f, 0.025592f),
                        Color(0.986502f, 0.623105f, 0.027814f),
                        Color(0.986964f, 0.630485f, 0.030908f),
                        Color(0.987337f, 0.637890f, 0.034916f),
                        Color(0.987622f, 0.645320f, 0.039886f),
                        Color(0.987819f, 0.652773f, 0.045581f),
                        Color(0.987926f, 0.660250f, 0.051750f),
                        Color(0.987945f, 0.667748f, 0.058329f),
                        Color(0.987874f, 0.675267f, 0.065257f),
                        Color(0.987714f, 0.682807f, 0.072489f),
                        Color(0.987464f, 0.690366f, 0.079990f),
                        Color(0.987124f, 0.697944f, 0.087731f),
                        Color(0.986694f, 0.705540f, 0.095694f),
                        Color(0.986175f, 0.713153f, 0.103863f),
                        Color(0.985566f, 0.720782f, 0.112229f),
                        Color(0.984865f, 0.728427f, 0.120785f),
                        Color(0.984075f, 0.736087f, 0.129527f),
                        Color(0.983196f, 0.743758f, 0.138453f),
                        Color(0.982228f, 0.751442f, 0.147565f),
                        Color(0.981173f, 0.759135f, 0.156863f),
                        Color(0.980032f, 0.766837f, 0.166353f),
                        Color(0.978806f, 0.774545f, 0.176037f),
                        Color(0.977497f, 0.782258f, 0.185923f),
                        Color(0.976108f, 0.789974f, 0.196018f),
                        Color(0.974638f, 0.797692f, 0.206332f),
                        Color(0.973088f, 0.805409f, 0.216877f),
                        Color(0.971468f, 0.813122f, 0.227658f),
                        Color(0.969783f, 0.820825f, 0.238686f),
                        Color(0.968041f, 0.828515f, 0.249972f),
                        Color(0.966243f, 0.836191f, 0.261534f),
                        Color(0.964394f, 0.843848f, 0.273391f),
                        Color(0.962517f, 0.851476f, 0.285546f),
                        Color(0.960626f, 0.859069f, 0.298010f),
                        Color(0.958720f, 0.866624f, 0.310820f),
                        Color(0.956834f, 0.874129f, 0.323974f),
                        Color(0.954997f, 0.881569f, 0.337475f),
                        Color(0.953215f, 0.888942f, 0.351369f),
                        Color(0.951546f, 0.896226f, 0.365627f),
                        Color(0.950018f, 0.903409f, 0.380271f),
                        Color(0.948683f, 0.910473f, 0.395289f),
                        Color(0.947594f, 0.917399f, 0.410665f),
                        Color(0.946809f, 0.924168f, 0.426373f),
                        Color(0.946392f, 0.930761f, 0.442367f),
                        Color(0.946403f, 0.937159f, 0.458592f),
                        Color(0.946903f, 0.943348f, 0.474970f),
                        Color(0.947937f, 0.949318f, 0.491426f),
                        Color(0.949545f, 0.955063f, 0.507860f),
                        Color(0.951740f, 0.960587f, 0.524203f),
                        Color(0.954529f, 0.965896f, 0.540361f),
                        Color(0.957896f, 0.971003f, 0.556275f),
                        Color(0.961812f, 0.975924f, 0.571925f),
                        Color(0.966249f, 0.980678f, 0.587206f),
                        Color(0.971162f, 0.985282f, 0.602154f),
                        Color(0.976511f, 0.989753f, 0.616760f),
                        Color(0.982257f, 0.994109f, 0.631017f),
                        Color(0.988362f, 0.998364f, 0.644924f),
                };
                break;
            case Style::CIVIDIS:
                colors = {
                        Color(0.0000f, 0.1262f, 0.3015f),
                        Color(0.0000f, 0.1292f, 0.3077f),
                        Color(0.0000f, 0.1321f, 0.3142f),
                        Color(0.0000f, 0.1350f, 0.3205f),
                        Color(0.0000f, 0.1379f, 0.3269f),
                        Color(0.0000f, 0.1408f, 0.3334f),
                        Color(0.0000f, 0.1437f, 0.3400f),
                        Color(0.0000f, 0.1465f, 0.3467f),
                        Color(0.0000f, 0.1492f, 0.3537f),
                        Color(0.0000f, 0.1519f, 0.3606f),
                        Color(0.0000f, 0.1546f, 0.3676f),
                        Color(0.0000f, 0.1574f, 0.3746f),
                        Color(0.0000f, 0.1601f, 0.3817f),
                        Color(0.0000f, 0.1629f, 0.3888f),
                        Color(0.0000f, 0.1657f, 0.3960f),
                        Color(0.0000f, 0.1685f, 0.4031f),
                        Color(0.0000f, 0.1714f, 0.4102f),
                        Color(0.0000f, 0.1743f, 0.4172f),
                        Color(0.0000f, 0.1773f, 0.4241f),
                        Color(0.0000f, 0.1798f, 0.4307f),
                        Color(0.0000f, 0.1817f, 0.4347f),
                        Color(0.0000f, 0.1834f, 0.4363f),
                        Color(0.0000f, 0.1852f, 0.4368f),
                        Color(0.0000f, 0.1872f, 0.4368f),
                        Color(0.0000f, 0.1901f, 0.4365f),
                        Color(0.0000f, 0.1930f, 0.4361f),
                        Color(0.0000f, 0.1958f, 0.4356f),
                        Color(0.0000f, 0.1987f, 0.4349f),
                        Color(0.0000f, 0.2015f, 0.4343f),
                        Color(0.0000f, 0.2044f, 0.4336f),
                        Color(0.0000f, 0.2073f, 0.4329f),
                        Color(0.0055f, 0.2101f, 0.4322f),
                        Color(0.0236f, 0.2130f, 0.4314f),
                        Color(0.0416f, 0.2158f, 0.4308f),
                        Color(0.0576f, 0.2187f, 0.4301f),
                        Color(0.0710f, 0.2215f, 0.4293f),
                        Color(0.0827f, 0.2244f, 0.4287f),
                        Color(0.0932f, 0.2272f, 0.4280f),
                        Color(0.1030f, 0.2300f, 0.4274f),
                        Color(0.1120f, 0.2329f, 0.4268f),
                        Color(0.1204f, 0.2357f, 0.4262f),
                        Color(0.1283f, 0.2385f, 0.4256f),
                        Color(0.1359f, 0.2414f, 0.4251f),
                        Color(0.1431f, 0.2442f, 0.4245f),
                        Color(0.1500f, 0.2470f, 0.4241f),
                        Color(0.1566f, 0.2498f, 0.4236f),
                        Color(0.1630f, 0.2526f, 0.4232f),
                        Color(0.1692f, 0.2555f, 0.4228f),
                        Color(0.1752f, 0.2583f, 0.4224f),
                        Color(0.1811f, 0.2611f, 0.4220f),
                        Color(0.1868f, 0.2639f, 0.4217f),
                        Color(0.1923f, 0.2667f, 0.4214f),
                        Color(0.1977f, 0.2695f, 0.4212f),
                        Color(0.2030f, 0.2723f, 0.4209f),
                        Color(0.2082f, 0.2751f, 0.4207f),
                        Color(0.2133f, 0.2780f, 0.4205f),
                        Color(0.2183f, 0.2808f, 0.4204f),
                        Color(0.2232f, 0.2836f, 0.4203f),
                        Color(0.2281f, 0.2864f, 0.4202f),
                        Color(0.2328f, 0.2892f, 0.4201f),
                        Color(0.2375f, 0.2920f, 0.4200f),
                        Color(0.2421f, 0.2948f, 0.4200f),
                        Color(0.2466f, 0.2976f, 0.4200f),
                        Color(0.2511f, 0.3004f, 0.4201f),
                        Color(0.2556f, 0.3032f, 0.4201f),
                        Color(0.2599f, 0.3060f, 0.4202f),
                        Color(0.2643f, 0.3088f, 0.4203f),
                        Color(0.2686f, 0.3116f, 0.4205f),
                        Color(0.2728f, 0.3144f, 0.4206f),
                        Color(0.2770f, 0.3172f, 0.4208f),
                        Color(0.2811f, 0.3200f, 0.4210f),
                        Color(0.2853f, 0.3228f, 0.4212f),
                        Color(0.2894f, 0.3256f, 0.4215f),
                        Color(0.2934f, 0.3284f, 0.4218f),
                        Color(0.2974f, 0.3312f, 0.4221f),
                        Color(0.3014f, 0.3340f, 0.4224f),
                        Color(0.3054f, 0.3368f, 0.4227f),
                        Color(0.3093f, 0.3396f, 0.4231f),
                        Color(0.3132f, 0.3424f, 0.4236f),
                        Color(0.3170f, 0.3453f, 0.4240f),
                        Color(0.3209f, 0.3481f, 0.4244f),
                        Color(0.3247f, 0.3509f, 0.4249f),
                        Color(0.3285f, 0.3537f, 0.4254f),
                        Color(0.3323f, 0.3565f, 0.4259f),
                        Color(0.3361f, 0.3593f, 0.4264f),
                        Color(0.3398f, 0.3622f, 0.4270f),
                        Color(0.3435f, 0.3650f, 0.4276f),
                        Color(0.3472f, 0.3678f, 0.4282f),
                        Color(0.3509f, 0.3706f, 0.4288f),
                        Color(0.3546f, 0.3734f, 0.4294f),
                        Color(0.3582f, 0.3763f, 0.4302f),
                        Color(0.3619f, 0.3791f, 0.4308f),
                        Color(0.3655f, 0.3819f, 0.4316f),
                        Color(0.3691f, 0.3848f, 0.4322f),
                        Color(0.3727f, 0.3876f, 0.4331f),
                        Color(0.3763f, 0.3904f, 0.4338f),
                        Color(0.3798f, 0.3933f, 0.4346f),
                        Color(0.3834f, 0.3961f, 0.4355f),
                        Color(0.3869f, 0.3990f, 0.4364f),
                        Color(0.3905f, 0.4018f, 0.4372f),
                        Color(0.3940f, 0.4047f, 0.4381f),
                        Color(0.3975f, 0.4075f, 0.4390f),
                        Color(0.4010f, 0.4104f, 0.4400f),
                        Color(0.4045f, 0.4132f, 0.4409f),
                        Color(0.4080f, 0.4161f, 0.4419f),
                        Color(0.4114f, 0.4189f, 0.4430f),
                        Color(0.4149f, 0.4218f, 0.4440f),
                        Color(0.4183f, 0.4247f, 0.4450f),
                        Color(0.4218f, 0.4275f, 0.4462f),
                        Color(0.4252f, 0.4304f, 0.4473f),
                        Color(0.4286f, 0.4333f, 0.4485f),
                        Color(0.4320f, 0.4362f, 0.4496f),
                        Color(0.4354f, 0.4390f, 0.4508f),
                        Color(0.4388f, 0.4419f, 0.4521f),
                        Color(0.4422f, 0.4448f, 0.4534f),
                        Color(0.4456f, 0.4477f, 0.4547f),
                        Color(0.4489f, 0.4506f, 0.4561f),
                        Color(0.4523f, 0.4535f, 0.4575f),
                        Color(0.4556f, 0.4564f, 0.4589f),
                        Color(0.4589f, 0.4593f, 0.4604f),
                        Color(0.4622f, 0.4622f, 0.4620f),
                        Color(0.4656f, 0.4651f, 0.4635f),
                        Color(0.4689f, 0.4680f, 0.4650f),
                        Color(0.4722f, 0.4709f, 0.4665f),
                        Color(0.4756f, 0.4738f, 0.4679f),
                        Color(0.4790f, 0.4767f, 0.4691f),
                        Color(0.4825f, 0.4797f, 0.4701f),
                        Color(0.4861f, 0.4826f, 0.4707f),
                        Color(0.4897f, 0.4856f, 0.4714f),
                        Color(0.4934f, 0.4886f, 0.4719f),
                        Color(0.4971f, 0.4915f, 0.4723f),
                        Color(0.5008f, 0.4945f, 0.4727f),
                        Color(0.5045f, 0.4975f, 0.4730f),
                        Color(0.5083f, 0.5005f, 0.4732f),
                        Color(0.5121f, 0.5035f, 0.4734f),
                        Color(0.5158f, 0.5065f, 0.4736f),
                        Color(0.5196f, 0.5095f, 0.4737f),
                        Color(0.5234f, 0.5125f, 0.4738f),
                        Color(0.5272f, 0.5155f, 0.4739f),
                        Color(0.5310f, 0.5186f, 0.4739f),
                        Color(0.5349f, 0.5216f, 0.4738f),
                        Color(0.5387f, 0.5246f, 0.4739f),
                        Color(0.5425f, 0.5277f, 0.4738f),
                        Color(0.5464f, 0.5307f, 0.4736f),
                        Color(0.5502f, 0.5338f, 0.4735f),
                        Color(0.5541f, 0.5368f, 0.4733f),
                        Color(0.5579f, 0.5399f, 0.4732f),
                        Color(0.5618f, 0.5430f, 0.4729f),
                        Color(0.5657f, 0.5461f, 0.4727f),
                        Color(0.5696f, 0.5491f, 0.4723f),
                        Color(0.5735f, 0.5522f, 0.4720f),
                        Color(0.5774f, 0.5553f, 0.4717f),
                        Color(0.5813f, 0.5584f, 0.4714f),
                        Color(0.5852f, 0.5615f, 0.4709f),
                        Color(0.5892f, 0.5646f, 0.4705f),
                        Color(0.5931f, 0.5678f, 0.4701f),
                        Color(0.5970f, 0.5709f, 0.4696f),
                        Color(0.6010f, 0.5740f, 0.4691f),
                        Color(0.6050f, 0.5772f, 0.4685f),
                        Color(0.6089f, 0.5803f, 0.4680f),
                        Color(0.6129f, 0.5835f, 0.4673f),
                        Color(0.6168f, 0.5866f, 0.4668f),
                        Color(0.6208f, 0.5898f, 0.4662f),
                        Color(0.6248f, 0.5929f, 0.4655f),
                        Color(0.6288f, 0.5961f, 0.4649f),
                        Color(0.6328f, 0.5993f, 0.4641f),
                        Color(0.6368f, 0.6025f, 0.4632f),
                        Color(0.6408f, 0.6057f, 0.4625f),
                        Color(0.6449f, 0.6089f, 0.4617f),
                        Color(0.6489f, 0.6121f, 0.4609f),
                        Color(0.6529f, 0.6153f, 0.4600f),
                        Color(0.6570f, 0.6185f, 0.4591f),
                        Color(0.6610f, 0.6217f, 0.4583f),
                        Color(0.6651f, 0.6250f, 0.4573f),
                        Color(0.6691f, 0.6282f, 0.4562f),
                        Color(0.6732f, 0.6315f, 0.4553f),
                        Color(0.6773f, 0.6347f, 0.4543f),
                        Color(0.6813f, 0.6380f, 0.4532f),
                        Color(0.6854f, 0.6412f, 0.4521f),
                        Color(0.6895f, 0.6445f, 0.4511f),
                        Color(0.6936f, 0.6478f, 0.4499f),
                        Color(0.6977f, 0.6511f, 0.4487f),
                        Color(0.7018f, 0.6544f, 0.4475f),
                        Color(0.7060f, 0.6577f, 0.4463f),
                        Color(0.7101f, 0.6610f, 0.4450f),
                        Color(0.7142f, 0.6643f, 0.4437f),
                        Color(0.7184f, 0.6676f, 0.4424f),
                        Color(0.7225f, 0.6710f, 0.4409f),
                        Color(0.7267f, 0.6743f, 0.4396f),
                        Color(0.7308f, 0.6776f, 0.4382f),
                        Color(0.7350f, 0.6810f, 0.4368f),
                        Color(0.7392f, 0.6844f, 0.4352f),
                        Color(0.7434f, 0.6877f, 0.4338f),
                        Color(0.7476f, 0.6911f, 0.4322f),
                        Color(0.7518f, 0.6945f, 0.4307f),
                        Color(0.7560f, 0.6979f, 0.4290f),
                        Color(0.7602f, 0.7013f, 0.4273f),
                        Color(0.7644f, 0.7047f, 0.4258f),
                        Color(0.7686f, 0.7081f, 0.4241f),
                        Color(0.7729f, 0.7115f, 0.4223f),
                        Color(0.7771f, 0.7150f, 0.4205f),
                        Color(0.7814f, 0.7184f, 0.4188f),
                        Color(0.7856f, 0.7218f, 0.4168f),
                        Color(0.7899f, 0.7253f, 0.4150f),
                        Color(0.7942f, 0.7288f, 0.4129f),
                        Color(0.7985f, 0.7322f, 0.4111f),
                        Color(0.8027f, 0.7357f, 0.4090f),
                        Color(0.8070f, 0.7392f, 0.4070f),
                        Color(0.8114f, 0.7427f, 0.4049f),
                        Color(0.8157f, 0.7462f, 0.4028f),
                        Color(0.8200f, 0.7497f, 0.4007f),
                        Color(0.8243f, 0.7532f, 0.3984f),
                        Color(0.8287f, 0.7568f, 0.3961f),
                        Color(0.8330f, 0.7603f, 0.3938f),
                        Color(0.8374f, 0.7639f, 0.3915f),
                        Color(0.8417f, 0.7674f, 0.3892f),
                        Color(0.8461f, 0.7710f, 0.3869f),
                        Color(0.8505f, 0.7745f, 0.3843f),
                        Color(0.8548f, 0.7781f, 0.3818f),
                        Color(0.8592f, 0.7817f, 0.3793f),
                        Color(0.8636f, 0.7853f, 0.3766f),
                        Color(0.8681f, 0.7889f, 0.3739f),
                        Color(0.8725f, 0.7926f, 0.3712f),
                        Color(0.8769f, 0.7962f, 0.3684f),
                        Color(0.8813f, 0.7998f, 0.3657f),
                        Color(0.8858f, 0.8035f, 0.3627f),
                        Color(0.8902f, 0.8071f, 0.3599f),
                        Color(0.8947f, 0.8108f, 0.3569f),
                        Color(0.8992f, 0.8145f, 0.3538f),
                        Color(0.9037f, 0.8182f, 0.3507f),
                        Color(0.9082f, 0.8219f, 0.3474f),
                        Color(0.9127f, 0.8256f, 0.3442f),
                        Color(0.9172f, 0.8293f, 0.3409f),
                        Color(0.9217f, 0.8330f, 0.3374f),
                        Color(0.9262f, 0.8367f, 0.3340f),
                        Color(0.9308f, 0.8405f, 0.3306f),
                        Color(0.9353f, 0.8442f, 0.3268f),
                        Color(0.9399f, 0.8480f, 0.3232f),
                        Color(0.9444f, 0.8518f, 0.3195f),
                        Color(0.9490f, 0.8556f, 0.3155f),
                        Color(0.9536f, 0.8593f, 0.3116f),
                        Color(0.9582f, 0.8632f, 0.3076f),
                        Color(0.9628f, 0.8670f, 0.3034f),
                        Color(0.9674f, 0.8708f, 0.2990f),
                        Color(0.9721f, 0.8746f, 0.2947f),
                        Color(0.9767f, 0.8785f, 0.2901f),
                        Color(0.9814f, 0.8823f, 0.2856f),
                        Color(0.9860f, 0.8862f, 0.2807f),
                        Color(0.9907f, 0.8901f, 0.2759f),
                        Color(0.9954f, 0.8940f, 0.2708f),
                        Color(1.0000f, 0.8979f, 0.2655f),
                        Color(1.0000f, 0.9018f, 0.2600f),
                        Color(1.0000f, 0.9057f, 0.2593f),
                        Color(1.0000f, 0.9094f, 0.2634f),
                        Color(1.0000f, 0.9131f, 0.2680f),
                        Color(1.0000f, 0.9169f, 0.2731f),
                };
                break;
            default:
                break;
        }
    }

    /*!
     * Evaluate this color map at the specified value.
     * \param value A number in range `[0.0f, 1.0f]`.
     * \return A `Color` representing the linearly interpolated value from this color map.
     */
    Color operator()(float value) const
    {
        const float lower_bound = 0.0f;
        const float upper_bound = 1.0f;

        if (value <= lower_bound)
            return colors.front();
        if (value >= upper_bound)
            return colors.back();

        value *= static_cast<float>(colors.size() - 1);
        const std::size_t lower_idx = static_cast<int>(value);

        value -= static_cast<float>(lower_idx);
        const Vec4 lower_rgba = colors[lower_idx].getRGBA();
        const Vec4 upper_rbga = colors[lower_idx + 1].getRGBA();
        const Vec4 interp_rgba =
                (value - lower_bound) * (upper_rbga - lower_rgba) / (upper_bound - lower_bound) + lower_rgba;

        return {interp_rgba};
    }

    /*!
     * Evaluate this color map at the specified value.
     * \param value A number in range `[0x00, 0xFF]`.
     * \return A `Color` representing the linearly interpolated value from this color map.
     */
    Color operator()(int value) const
    {
        return (*this)(static_cast<float>(value) / 255.0f);
    }

private:
    std::vector<Color> colors;  ///< Equally-spaced list of colors to interpolate between when evaluating the color map.
};

class Orient3
{
public:
	Orient3()
	: q(Eigen::Quaternionf::Identity())
	{}
	Orient3(const Eigen::Quaternionf& p_q)
	: q(p_q)
	{}
	Orient3(const Vec3& r)
	{
		float angle = r.norm();
		Vec3 axis;
		if (angle > 1e-3)
			axis = r / angle;
		else
			axis = Vec3(1,0,0);

		q = Eigen::Quaternionf(Eigen::AngleAxisf(angle, axis));
	}
	static Orient3 fromMatrix(const Mat3& R)
	{
		Eigen::AngleAxisf aa;
		aa.fromRotationMatrix(R);
		return Orient3(Eigen::Quaternionf(aa));
	}
	Orient3 inverse() const
	{
		return Orient3(q.inverse());
	}
	void normalize()
	{
		q.normalize();
	}
	Eigen::Quaternionf quat() const
	{
		return q;
	}
	Vec3 rvec() const
	{
		Eigen::AngleAxisf aa(q);
		return aa.angle() * aa.axis();
	}
	Orient3 operator* (const Orient3& other) const
	{
		return q * other.q;
	}
	Vec3 operator* (const Vec3& v) const
	{
		return q * v;
	}
private:
	Eigen::Quaternionf q;
};

class Transform3
{
public:
	Transform3()
	: transform(manif::SE3f::Identity())
	{}
	Transform3(const Vec3& t, const Orient3& r)
	: transform(t, r.quat())
	{}
	Transform3(const Vec3& t, const Vec3& r = Vec3::Zero())
	: transform(t, Orient3(r).quat())
	{}
	Transform3(const manif::SE3f& p_transform)
	: transform(p_transform)
	{}
	Transform3 inverse() const
	{
		return Transform3(transform.inverse());
	}
	Mat4 matrix() const
	{
		return transform.transform();
	}
	Mat3 rotation() const
	{
		return transform.rotation();
	}
	Orient3 quat() const
	{
		return Orient3(transform.quat());
	}
	Vec3 rvec() const
	{
		Eigen::AngleAxisf aa(transform.quat());
		return aa.angle() * aa.axis();
	}
	Transform3 operator* (const Transform3& other) const
	{
		return transform * other.transform;
	}
	Vec3 operator* (const Vec3& v) const
	{
		return transform.act(v);
	}
	Vec3 translation() const
	{
		return transform.translation();
	}
	void setTranslation(const Vec3& t)
	{
		transform.translation(t);
	}
	void setRotation(const Orient3& o)
	{
		transform.quat(o.quat());
	}
	manif::SE3f manifoldTransform() const
	{
		return transform;
	}
private:
	manif::SE3f transform;
};

struct TransformSim3
{
	TransformSim3()
	{}
	TransformSim3(const Transform3& p_transform, float p_scale = 1.0f)
	: transform(p_transform), scale(p_scale)
	{}
	TransformSim3(const Vec3& p_t, const Vec3& p_r = Vec3::Zero(), float p_scale = 1.0f)
	: transform(p_t,p_r), scale(p_scale)
	{}
	TransformSim3 operator* (const TransformSim3& other) const
	{
		Transform3 scaled_transform = other.transform;
		scaled_transform.setTranslation(scaled_transform.translation() * scale);
		
		return TransformSim3(transform * scaled_transform, scale * other.scale);
	}
	TransformSim3 operator* (const Transform3& other) const
	{
		Transform3 scaled_transform = other;
		scaled_transform.setTranslation(scaled_transform.translation() * scale);
		
		return TransformSim3(transform * scaled_transform, scale);
	}
	Vec3 operator* (const Vec3& v) const
	{
		return transform * (scale * v);
	}
	Vec3 translation() const
	{
		return transform.translation();
	}
	Mat3 rotation() const
	{
		return transform.rotation();
	}
	Orient3 quat() const
	{
		return Orient3(transform.quat());
	}
	void setTranslation(const Vec3& t)
	{
		transform.setTranslation(t);
	}
	void setRotation(const Orient3& o)
	{
		transform.setRotation(o.quat());
	}
	Mat4 matrix() const
	{
		Mat4 m = transform.matrix();
		m.block<3,3>(0,0) *= scale;
		return m;
	}
	
	Transform3 transform;
	float scale = 1.0f;
};

inline float sign(float v)
{
    return v >= 0 ? 1 : -1;
}

inline float dot2(const Vec3& v)
{
    return v.dot(v);
}

inline float clamp(float v, float vmin, float vmax)
{
    return min(max(v, vmin), vmax);
}

inline Vec3 makeFacing(Vec3 facing, Vec3 base_facing = Vec3(0,0,1))
{
	facing = facing / facing.norm();
	base_facing = base_facing / base_facing.norm();
	
	Vec3 axis = base_facing.cross(facing);
	float axis_mag = axis.norm();
	if (axis_mag > 1e-3)
		axis /= axis_mag;
	else
		axis = Vec3(1,0,0);
	
	float angle = acos(clamp(facing.dot(base_facing), -1.0f, 1.0f));
	
	return angle * axis;
}

inline MatX toMat(const vector<Vec2>& v)
{
	MatX mat(v.size(), 2);
	
	for (int i = 0; i < v.size(); i++)
	{
		mat.row(i) = v[i].transpose();
	}
	
	return mat;
}

inline MatX toMat(const vector<Vec3>& v)
{
	MatX mat(v.size(), 3);
	
	for (int i = 0; i < v.size(); i++)
	{
		mat.row(i) = v[i].transpose();
	}
	
	return mat;
}

inline MatX toMat(const vector<Vec4>& v)
{
	MatX mat(v.size(), 4);
	
	for (int i = 0; i < v.size(); i++)
	{
		mat.row(i) = v[i].transpose();
	}
	
	return mat;
}

inline Vec3 findCrossVec(const Vec3& v)
{
    vector<Vec3> test_vecs = {
        Vec3(1,0,0),
        Vec3(0,1,0),
        Vec3(0,0,1),
    };

    float min_dval = 2.0;
    Vec3 min_v;

    for (const auto& other_v : test_vecs)
    {
        float dval = fabs(other_v.dot(v));
        if (dval < min_dval)
        {
            min_dval = dval;
            min_v = other_v;
        }
    }

    min_v = v.cross(min_v).cross(v);
    min_v /= min_v.norm();
    return min_v;
}

inline Mat3 skew(const Vec3& v)
{
    Mat3 out = Mat3::Zero();

    out(2,1) = v[0];
    out(2,0) = -v[1];
    out(1,0) = v[2];

    out(1,2) = -v[0];
    out(0,2) = v[1];
    out(0,1) = -v[2];

    return out;
}

inline Mat3 rodrigues(const Vec3& v)
{
    float angle = v.norm();
    if (angle == 0)
        return Mat3::Identity();
    Vec3 axis = v / angle;
    Mat3 skew_axis = skew(axis);

    return Mat3::Identity() + sin(angle)*skew_axis + (1-cos(angle))*skew_axis*skew_axis;
}

}
