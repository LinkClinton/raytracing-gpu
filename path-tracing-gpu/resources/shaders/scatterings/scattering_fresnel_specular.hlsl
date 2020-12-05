#ifndef __SCATTERING_FRESNEL_SPECULAR_HLSL__
#define __SCATTERING_FRESNEL_SPECULAR_HLSL__

#include "scattering_fresnel_effect.hlsl"

#define __ENABLE_SPECULAR_TRANSMISSION__

float3 evaluate_fresnel_specular()
{
	return 0;
}

scattering_sample sample_fresnel_specular(float3 transmission, float3 reflectance, float input_eta_i, float input_eta_o,
	float3 wo, float2 value)
{
	scattering_sample sample;

	sample.pdf = 0;
	
	float fresnel = fresnel_reflect_dielectric(cos_theta(wo), input_eta_i, input_eta_o);

	if (value.x < fresnel) {
		sample.type = scattering_type(scattering_reflection | scattering_specular);
		sample.wi = float3(-wo.x, -wo.y, wo.z);
		sample.value = reflectance * fresnel / abs(cos_theta(sample.wi));
		sample.pdf = fresnel;

		return sample;
	}

	bool entering = cos_theta(wo) > 0;
	float eta_i = entering ? input_eta_i : input_eta_o;
	float eta_o = entering ? input_eta_o : input_eta_i;

	float3 wi = not_default_refract(wo, face_forward(vector3(0, 0, 1), wo), eta_i / eta_o);

	if (wi.x == 0 && wi.y == 0 && wi.z == 0) return sample;

	float3 factor = (eta_i * eta_i) / (eta_o * eta_o);

	sample.type = scattering_type(scattering_transmission | scattering_specular);
	sample.value = transmission * (1 - fresnel) * factor / abs(cos_theta(wi));
	sample.wi = wi;
	sample.pdf = 1 - fresnel;

	return sample;
}

float pdf_fresnel_specular()
{
	return 0;
}

#endif