#ifndef AUDIOSTREAMSAMPLE_H
#define AUDIOSTREAMSAMPLE_H

#include "servers/audio/audio_stream.h"


class AudioStreamSample;

class AudioStreamPlaybackSample : public AudioStreamPlayback {

	GDCLASS( AudioStreamPlaybackSample, AudioStreamPlayback )
	enum {
		MIX_FRAC_BITS=13,
		MIX_FRAC_LEN=(1<<MIX_FRAC_BITS),
		MIX_FRAC_MASK=MIX_FRAC_LEN-1,
	};

	struct IMA_ADPCM_State {

		int16_t step_index;
		int32_t predictor;
		/* values at loop point */
		int16_t loop_step_index;
		int32_t loop_predictor;
		int32_t last_nibble;
		int32_t loop_pos;
		int32_t window_ofs;
		const uint8_t *ptr;
	} ima_adpcm[2];

	int64_t offset;
	int sign;
	bool active;
friend class AudioStreamSample;
	Ref<AudioStreamSample> base;

	template<class Depth,bool is_stereo,bool is_ima_adpcm>
	void do_resample(const Depth* p_src, AudioFrame *p_dst,int64_t &offset,int32_t &increment,uint32_t amount,IMA_ADPCM_State *ima_adpcm);
public:

	virtual void start(float p_from_pos=0.0);
	virtual void stop();
	virtual bool is_playing() const;

	virtual int get_loop_count() const; //times it looped

	virtual float get_pos() const;
	virtual void seek_pos(float p_time);

	virtual void mix(AudioFrame* p_buffer,float p_rate_scale,int p_frames);

	virtual float get_length() const; //if supported, otherwise return 0


	AudioStreamPlaybackSample();
};

class AudioStreamSample : public AudioStream {
	GDCLASS(AudioStreamSample,AudioStream)
	RES_BASE_EXTENSION("smp")

public:

	enum Format {
		FORMAT_8_BITS,
		FORMAT_16_BITS,
		FORMAT_IMA_ADPCM
	};

	enum LoopMode {
		LOOP_DISABLED,
		LOOP_FORWARD,
		LOOP_PING_PONG
	};


private:
friend class AudioStreamPlaybackSample;

	enum {
		DATA_PAD=16 //padding for interpolation
	};

	Format format;
	LoopMode loop_mode;
	bool stereo;
	int loop_begin;
	int loop_end;
	int mix_rate;
	void *data;
	uint32_t data_bytes;
protected:

	static void _bind_methods();
public:
	void set_format(Format p_format);
	Format get_format() const;

	void set_loop_mode(LoopMode p_loop_mode);
	LoopMode get_loop_mode() const;

	void set_loop_begin(int p_frame);
	int get_loop_begin() const;

	void set_loop_end(int p_frame);
	int get_loop_end() const;

	void set_mix_rate(int p_hz);
	int get_mix_rate() const;

	void set_stereo(bool p_enable);
	bool is_stereo() const;

	void set_data(const PoolVector<uint8_t>& p_data);
	PoolVector<uint8_t> get_data() const;


	virtual Ref<AudioStreamPlayback> instance_playback();
	virtual String get_stream_name() const;

	AudioStreamSample();
	~AudioStreamSample();
};

VARIANT_ENUM_CAST(AudioStreamSample::Format)
VARIANT_ENUM_CAST(AudioStreamSample::LoopMode)

#endif // AUDIOSTREAMSample_H
