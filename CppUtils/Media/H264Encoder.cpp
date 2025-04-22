#include "H264Encoder.h"
#include <Utils/Utils.h>
#pragma comment(lib, "mfreadwrite")
#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")

#define BREAK_ON_FAIL(value)            if(FAILED(value)) break;
#define BREAK_ON_NULL(value, newHr)     if(value == NULL) { hr = newHr; break; }
bool H264Encoder::WMFLIB_INITED = false;
static void configWMFLib()
{
	if (!H264Encoder::WMFLIB_INITED)
	{
		CoInitialize(NULL);
		CoUninitialize();
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		MFStartup(MF_VERSION);
		H264Encoder::WMFLIB_INITED = true;
	}

}
AudioReader::AudioReader(const std::wstring& filePath) : audioFilename(filePath), m_pSourceReader(NULL)
{
	configWMFLib();
	HRESULT hr;
	IMFMediaType* pPartialMediaType = NULL;
	do
	{
		hr = MFCreateSourceReaderFromURL(audioFilename.c_str(), NULL, &m_pSourceReader);
		BREAK_ON_FAIL(hr);
		hr = GetSourceInfomation();
		BREAK_ON_FAIL(hr);
		hr = MFCreateMediaType(&pPartialMediaType);
		BREAK_ON_FAIL(hr);
		hr = pPartialMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		BREAK_ON_FAIL(hr);
		hr = pPartialMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float);
		BREAK_ON_FAIL(hr);
		hr = this->SetCurrentMediaType(0, NULL, pPartialMediaType);
		pPartialMediaType->Release();
	} while (false);
}
AudioReader::AudioReader(const std::vector<BYTE>& fileBuffer) : audioFilename(L""), m_pSourceReader(NULL)
{
	configWMFLib();
	HRESULT hr;
	IMFMediaType* pPartialMediaType = NULL;
	do
	{
		IStream* pStream = SHCreateMemStream(fileBuffer.data(), static_cast<UINT>(fileBuffer.size()));
		hr = MFCreateMFByteStreamOnStream(pStream, &pByteStream);
		BREAK_ON_FAIL(hr);
		pStream->Release();
		hr = MFCreateSourceReaderFromByteStream(pByteStream, NULL, &m_pSourceReader);
		BREAK_ON_FAIL(hr);
		hr = GetSourceInfomation();
		BREAK_ON_FAIL(hr);
		hr = MFCreateMediaType(&pPartialMediaType);
		BREAK_ON_FAIL(hr);
		hr = pPartialMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		BREAK_ON_FAIL(hr);
		hr = pPartialMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float);
		BREAK_ON_FAIL(hr);
		hr = this->SetCurrentMediaType(0, NULL, pPartialMediaType);
		pPartialMediaType->Release();
	} while (false);
}
AudioReader::AudioReader(IMFMediaSource* mediaSource)
{
	configWMFLib();
	HRESULT hr;
	IMFMediaType* pPartialMediaType = NULL;
	do
	{
		hr = MFCreateSourceReaderFromMediaSource(mediaSource, NULL, &m_pSourceReader);
		BREAK_ON_FAIL(hr);
		hr = GetSourceInfomation();
		BREAK_ON_FAIL(hr);
		hr = MFCreateMediaType(&pPartialMediaType);
		BREAK_ON_FAIL(hr);
		hr = pPartialMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		BREAK_ON_FAIL(hr);
		hr = pPartialMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float);
		BREAK_ON_FAIL(hr);
		hr = this->SetCurrentMediaType(0, NULL, pPartialMediaType);
		pPartialMediaType->Release();
	} while (false);
}
AudioReader::~AudioReader() {
	if (m_pSourceReader)
		m_pSourceReader->Release();
	if (pByteStream)
		pByteStream->Release();
}
IMFMediaType* AudioReader::GetCurrentMediaType(UINT32 index)
{
	IMFMediaType* result = NULL;
	HRESULT hr = m_pSourceReader->GetCurrentMediaType(index, &result);
	if (FAILED(hr))
		return NULL;
	return result;
}
HRESULT AudioReader::GetSourceInfomation()
{
	EncodingBitRate = NULL;
	Duration = NULL;
	IMFPresentationDescriptor* pPD = nullptr;
	IMFMediaSource* pSource = nullptr;
	HRESULT hr = m_pSourceReader->GetServiceForStream(
		MF_SOURCE_READER_MEDIASOURCE,
		GUID_NULL, IID_IMFMediaSource,
		(void**)&pSource
	);
	if (FAILED(hr))
		return hr;
	hr = pSource->CreatePresentationDescriptor(&pPD);
	if (SUCCEEDED(hr))
	{

		hr = pPD->GetUINT32(MF_PD_AUDIO_ENCODING_BITRATE, &EncodingBitRate);
		hr = pPD->GetUINT64(MF_PD_DURATION, &Duration);
		hr = pPD->GetUINT64(MF_PD_TOTAL_FILE_SIZE, &FileSize);
		pPD->Release();

		IMFMediaType* pMediaType = this->GetCurrentMediaType(0);
		hr = pMediaType->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &ChannelCount);
		hr = pMediaType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &SamplesPerSecond);
		hr = pMediaType->GetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, &AverageBytesPerSecond);
		hr = pMediaType->GetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, &AudioBlockAlignment);
		hr = pMediaType->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &AudioBitsPerSample);
		hr = pMediaType->GetGUID(MF_MT_SUBTYPE, &SubType);
		hr = pMediaType->GetGUID(MF_MT_MAJOR_TYPE, &MediaType);
		pMediaType->Release();
		pSource->Release();
	}
	return hr;
}
IMFSample* AudioReader::ReadNextSample() {
	LONGLONG timestamp = 0;
	DWORD streamIndex;
	DWORD sampleflag;
	IMFSample* sample;
	HRESULT hr = m_pSourceReader->ReadSample(
		MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &streamIndex, &sampleflag, &timestamp, &sample);
	return SUCCEEDED(hr) && sample ? sample : NULL;
}
HRESULT AudioReader::SetPosition(LONGLONG timeStamp)
{
	PROPVARIANT var;
	PropVariantInit(&var);
	var.vt = VT_I8;
	var.hVal.QuadPart = timeStamp;
	return m_pSourceReader->SetCurrentPosition(GUID_NULL, var);
}
double AudioReader::LenghtOfSeconds() const {
	return  (double)Duration / 10000000.0;
}
IMFMediaType* AudioReader::GetNativeMediaType(UINT32 index)
{
	IMFMediaType* pStreamMediaType = NULL;
	HRESULT hr = m_pSourceReader->GetNativeMediaType(
		index, 0, &pStreamMediaType);
	if (FAILED(hr))
		return  NULL;
	return pStreamMediaType;
}
HRESULT AudioReader::SetCurrentMediaType(DWORD dwStreamIndex, DWORD* pdwReserved, IMFMediaType* pMediaType)
{
	return m_pSourceReader->SetCurrentMediaType(dwStreamIndex, pdwReserved, pMediaType);
}

std::vector<MixedFrame> AudioReader::ReadAudioFrames(IMFMediaType* targetMediaType)
{
	HRESULT hr = S_OK;
	std::vector<MixedFrame> frames;

	IMFMediaType* pCurrentMediaType = this->GetCurrentMediaType(0);

	AudioResampler* resampler = nullptr;
	bool needResample = false;

	if (targetMediaType)
	{
		BOOL isEqual = FALSE;
		hr = pCurrentMediaType->Compare(targetMediaType, MF_ATTRIBUTES_MATCH_INTERSECTION, &isEqual);

		if (FAILED(hr) || !isEqual)
		{
			needResample = true;
			resampler = new AudioResampler(pCurrentMediaType, targetMediaType);
		}
	}

	pCurrentMediaType->Release();

	IMFSample* sample = nullptr;
	while ((sample = this->ReadNextSample()) != nullptr)
	{
		IMFSample* processedSample = sample;

		if (needResample)
		{
			hr = resampler->Resample(sample, &processedSample);
			if (FAILED(hr))
			{
				sample->Release();
				break;
			}
		}

		LONGLONG time = 0;
		LONGLONG duration = 0;
		processedSample->GetSampleTime(&time);
		processedSample->GetSampleDuration(&duration);


		IMFMediaBuffer* pInputBuffer = nullptr;
		hr = processedSample->ConvertToContiguousBuffer(&pInputBuffer);
		if (FAILED(hr))
			continue;

		BYTE* pInputData = nullptr;
		DWORD inputDataSize = 0;
		hr = pInputBuffer->Lock(&pInputData, NULL, &inputDataSize);
		if (FAILED(hr))
			continue;


		float* pData = (float*)pInputData;
		DWORD dataSize = inputDataSize;
		size_t sampleCount = dataSize / sizeof(float);

		std::vector<float> data(pData, pData + sampleCount);

		pInputBuffer->Unlock();
		pInputBuffer->Release();

		MixedFrame frame;
		frame.Data = data;
		frame.Time = time;
		frame.Duration = duration;

		frames.push_back(frame);

		if (processedSample != sample)
			processedSample->Release();

		sample->Release();
	}

	if (resampler)
		delete resampler;

	return frames;
}
AudioResampler::AudioResampler(IMFMediaType* pInputType, IMFMediaType* pOutputType)
	: m_pResampler(nullptr)
{
	HRESULT hr = S_OK;

	//妈的不知道在哪个lib里定义的，自己写吧(CLSID_CResamplerMediaObject)
	auto CLSID_CResamplerMediaObject_GUID = Guid::Parse("f447b69e-1884-4a7e-8055-346f74d6edb3").Data;
	// 创建 Resampler MFT
	hr = CoCreateInstance(
		CLSID_CResamplerMediaObject_GUID,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&m_pResampler)
	);

	if (SUCCEEDED(hr))
	{
		hr = m_pResampler->SetInputType(0, pInputType, 0);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pResampler->SetOutputType(0, pOutputType, 0);
	}
}
AudioResampler::~AudioResampler()
{
	if (m_pResampler)
	{
		m_pResampler->Release();
	}
}
HRESULT AudioResampler::Resample(IMFSample* pInputSample, IMFSample** ppOutputSample)
{
	HRESULT hr = S_OK;
	MFT_OUTPUT_DATA_BUFFER outputDataBuffer;
	DWORD status = 0;

	// 提交输入样本
	hr = m_pResampler->ProcessInput(0, pInputSample, 0);

	if (SUCCEEDED(hr))
	{
		// 初始化输出缓冲区
		ZeroMemory(&outputDataBuffer, sizeof(outputDataBuffer));
		outputDataBuffer.pSample = nullptr;

		// 创建输出样本
		hr = MFCreateSample(&(outputDataBuffer.pSample));
	}

	if (SUCCEEDED(hr))
	{
		// 创建媒体缓冲区
		IMFMediaBuffer* pBuffer = nullptr;
		DWORD outputBufferSize = 0;

		// 估计输出缓冲区大小
		IMFMediaType* pOutputType = nullptr;
		hr = m_pResampler->GetOutputCurrentType(0, &pOutputType);

		if (SUCCEEDED(hr))
		{
			UINT32 sampleRate = 0, channelCount = 0, bitsPerSample = 0;
			hr = pOutputType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &sampleRate);
			hr = pOutputType->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &channelCount);
			hr = pOutputType->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &bitsPerSample);

			// 计算缓冲区大小
			LONGLONG duration = 0;
			pInputSample->GetSampleDuration(&duration);

			UINT64 numSamples = (UINT64)(sampleRate * duration / 10000000); // 将持续时间转换为秒
			outputBufferSize = (DWORD)(numSamples * channelCount * (bitsPerSample / 8));

			pOutputType->Release();
		}

		if (SUCCEEDED(hr))
		{
			hr = MFCreateMemoryBuffer(outputBufferSize, &pBuffer);
		}

		if (SUCCEEDED(hr))
		{
			hr = outputDataBuffer.pSample->AddBuffer(pBuffer);
			pBuffer->Release();
		}
	}

	if (SUCCEEDED(hr))
	{
		// 处理输出
		hr = m_pResampler->ProcessOutput(0, 1, &outputDataBuffer, &status);

		if (hr == MF_E_TRANSFORM_NEED_MORE_INPUT)
		{
			// 需要更多输入数据
			hr = S_OK;
		}
		else if (SUCCEEDED(hr))
		{
			*ppOutputSample = outputDataBuffer.pSample;
			(*ppOutputSample)->AddRef();
		}
	}

	if (outputDataBuffer.pSample)
	{
		outputDataBuffer.pSample->Release();
	}

	return hr;
}
/*
将两个音频按权重进行混合
mainVolume:主音频音量权重[0-1]
secondVolume:副音频音量权重[0-1]
clipToMainAudio:是否将副音频剪切到匹配主音频的长度
loopMode:如果副音频长度比主音频短则循环副音频
*/
std::vector<MixedFrame> AudioMix::MixAudio(AudioReader* mainAudio, float mainVolume, AudioReader* second, float secondVolume, bool clipToMainAudio, bool loopMode)
{
	// 获取第一个音频的媒体类型
	IMFMediaType* pFirstMediaType = mainAudio->GetCurrentMediaType(0);

	// 将两个音频读取为 MixedFrame 序列
	std::vector<MixedFrame> framesFirst = mainAudio->ReadAudioFrames(NULL);
	std::vector<MixedFrame> framesSecond = second->ReadAudioFrames(pFirstMediaType);

	// 释放媒体类型
	pFirstMediaType->Release();

	// 获取采样率和通道数，用于时间计算
	UINT32 sampleRate = mainAudio->SamplesPerSecond;
	UINT32 channels = mainAudio->ChannelCount;
	double samplesPerSecond = static_cast<double>(sampleRate * channels);

	// 计算第一个音频的总样本数
	size_t totalSamplesFirst = 0;
	for (const auto& frame : framesFirst)
	{
		totalSamplesFirst += frame.Data.size();
	}

	// 计算第二个音频的总样本数
	size_t totalSamplesSecond = 0;
	for (const auto& frame : framesSecond)
	{
		totalSamplesSecond += frame.Data.size();
	}

	// 总样本数
	size_t totalSamples = max(totalSamplesFirst, totalSamplesSecond);

	// 创建混合后的样本数据容器
	std::vector<float> mixedData;
	mixedData.reserve(totalSamples);

	// 使用迭代器遍历两个音频的样本数据
	size_t indexFirst = 0, indexSecond = 0;
	size_t frameIndexFirst = 0, frameIndexSecond = 0;
	size_t sampleIndexFirst = 0, sampleIndexSecond = 0;
	float maxVal = 0.0f, minVal = 0.0f;

	while (indexFirst < totalSamplesFirst || indexSecond < totalSamplesSecond)
	{
		float sampleValue = 0.0f;

		// 从第一个音频获取样本值
		if (indexFirst < totalSamplesFirst)
		{
			if (sampleIndexFirst >= framesFirst[frameIndexFirst].Data.size())
			{
				// 移动到下一个帧
				frameIndexFirst++;
				sampleIndexFirst = 0;
			}
			if (frameIndexFirst < framesFirst.size())
			{
				sampleValue += framesFirst[frameIndexFirst].Data[sampleIndexFirst++] * mainVolume;
				indexFirst++;
			}
		}

		if (loopMode)
		{
			if (indexFirst >= totalSamplesFirst)
				break;
			indexSecond = 0;
			frameIndexSecond = 0;
			sampleIndexSecond = 0;
		}

		// 从第二个音频获取样本值
		if (indexSecond < totalSamplesSecond)
		{
			if (sampleIndexSecond >= framesSecond[frameIndexSecond].Data.size())
			{
				// 移动到下一个帧
				frameIndexSecond++;
				sampleIndexSecond = 0;
			}
			if (frameIndexSecond < framesSecond.size())
			{
				sampleValue += framesSecond[frameIndexSecond].Data[sampleIndexSecond++] * secondVolume;
				indexSecond++;
			}
		}

		if (sampleValue > maxVal)
			maxVal = sampleValue;
		if (sampleValue < minVal)
			minVal = sampleValue;

		// 添加混合后的样本值
		mixedData.push_back(sampleValue);

		if (clipToMainAudio && indexFirst >= totalSamplesFirst)
			break;
	}
	if (maxVal > 0.75f || minVal < -0.75f)
	{
		float maxMul = max(maxVal, -minVal);
		for (auto& fm : mixedData)
		{
			fm /= maxMul;
		}
	}

	// 将混合后的数据分割为帧
	std::vector<MixedFrame> result;
	size_t samplesPerFrame = framesFirst[0].Data.size(); // 每个帧的样本数
	size_t totalMixedSamples = mixedData.size();
	size_t currentSampleIndex = 0;
	size_t currFrameIndex = 0;
	while (currentSampleIndex < totalMixedSamples)
	{
		if (currFrameIndex < framesFirst.size())
			samplesPerFrame = framesFirst[currFrameIndex++].Data.size();
		MixedFrame frame;
		size_t remainingSamples = totalMixedSamples - currentSampleIndex;
		size_t currentFrameSamples = min(samplesPerFrame, remainingSamples);

		// 复制样本数据到当前帧
		frame.Data.insert(frame.Data.end(),
			mixedData.begin() + currentSampleIndex,
			mixedData.begin() + currentSampleIndex + currentFrameSamples);

		// 计算当前帧的持续时间（单位：100纳秒）
		frame.Duration = static_cast<LONGLONG>((currentFrameSamples * 10000000) / samplesPerSecond);

		// 设置时间戳（可选，如果需要）
		frame.Time = static_cast<LONGLONG>((currentSampleIndex * 10000000) / samplesPerSecond);

		// 添加帧到结果中
		result.push_back(frame);

		// 更新样本索引
		currentSampleIndex += currentFrameSamples;
	}

	return result;
}
/*
将一个主音频和多个连续的副音频按权重进行混合
mainVolume:主音频音量权重[0-1]
secondVolume:副音频音量权重[0-1]
clipToMainAudio:是否将副音频剪切到匹配主音频的长度
loopMode:如果副音频长度比主音频短则循环副音频
*/
std::vector<MixedFrame> AudioMix::MixAudio(AudioReader* mainAudio, float mainVolume, const std::vector<AudioReader*>& secondList, float secondVolume, bool clipToMainAudio, bool loopMode)
{
	// 获取第一个音频的媒体类型
	IMFMediaType* pFirstMediaType = mainAudio->GetCurrentMediaType(0);

	// 将两个音频读取为 MixedFrame 序列
	std::vector<MixedFrame> framesFirst = mainAudio->ReadAudioFrames(NULL);
	std::vector<MixedFrame> framesSecond;
	for (auto sec : secondList)
	{
		std::vector<MixedFrame> secTmp = sec->ReadAudioFrames(pFirstMediaType);
		framesSecond.insert(framesSecond.end(), secTmp.begin(), secTmp.end());
	}
	// 释放媒体类型
	pFirstMediaType->Release();

	// 获取采样率和通道数，用于时间计算
	UINT32 sampleRate = mainAudio->SamplesPerSecond;
	UINT32 channels = mainAudio->ChannelCount;
	double samplesPerSecond = static_cast<double>(sampleRate * channels);

	// 计算第一个音频的总样本数
	size_t totalSamplesFirst = 0;
	for (const auto& frame : framesFirst)
	{
		totalSamplesFirst += frame.Data.size();
	}

	// 计算第二个音频的总样本数
	size_t totalSamplesSecond = 0;
	for (const auto& frame : framesSecond)
	{
		totalSamplesSecond += frame.Data.size();
	}

	// 总样本数
	size_t totalSamples = max(totalSamplesFirst, totalSamplesSecond);

	// 创建混合后的样本数据容器
	std::vector<float> mixedData;
	mixedData.reserve(totalSamples);

	// 使用迭代器遍历两个音频的样本数据
	size_t indexFirst = 0, indexSecond = 0;
	size_t frameIndexFirst = 0, frameIndexSecond = 0;
	size_t sampleIndexFirst = 0, sampleIndexSecond = 0;
	float maxVal = 0.0f, minVal = 0.0f;

	while (indexFirst < totalSamplesFirst || indexSecond < totalSamplesSecond)
	{
		float sampleValue = 0.0f;

		// 从第一个音频获取样本值
		if (indexFirst < totalSamplesFirst)
		{
			if (sampleIndexFirst >= framesFirst[frameIndexFirst].Data.size())
			{
				// 移动到下一个帧
				frameIndexFirst++;
				sampleIndexFirst = 0;
			}
			if (frameIndexFirst < framesFirst.size())
			{
				sampleValue += framesFirst[frameIndexFirst].Data[sampleIndexFirst++] * mainVolume;
				indexFirst++;
			}
		}

		if (loopMode && indexSecond >= totalSamplesSecond)
		{
			indexSecond = 0;
			frameIndexSecond = 0;
			sampleIndexSecond = 0;
		}

		// 从第二个音频获取样本值
		if (indexSecond < totalSamplesSecond)
		{
			if (sampleIndexSecond >= framesSecond[frameIndexSecond].Data.size())
			{
				// 移动到下一个帧
				frameIndexSecond++;
				sampleIndexSecond = 0;
			}
			if (frameIndexSecond < framesSecond.size())
			{
				sampleValue += framesSecond[frameIndexSecond].Data[sampleIndexSecond++] * secondVolume;
				indexSecond++;
			}
		}

		if (sampleValue > maxVal)
			maxVal = sampleValue;
		if (sampleValue < minVal)
			minVal = sampleValue;

		// 添加混合后的样本值
		mixedData.push_back(sampleValue);

		if (clipToMainAudio && indexFirst >= totalSamplesFirst)
			break;
	}
	if (maxVal > 0.75f || minVal < -0.75f)
	{
		float maxMul = max(maxVal, -minVal);
		for (auto& fm : mixedData)
		{
			fm /= maxMul;
		}
	}

	// 将混合后的数据分割为帧
	std::vector<MixedFrame> result;
	size_t samplesPerFrame = framesFirst[0].Data.size(); // 每个帧的样本数
	size_t totalMixedSamples = mixedData.size();
	size_t currentSampleIndex = 0;
	size_t currFrameIndex = 0;
	while (currentSampleIndex < totalMixedSamples)
	{
		if (currFrameIndex < framesFirst.size())
			samplesPerFrame = framesFirst[currFrameIndex++].Data.size();
		MixedFrame frame;
		size_t remainingSamples = totalMixedSamples - currentSampleIndex;
		size_t currentFrameSamples = min(samplesPerFrame, remainingSamples);

		// 复制样本数据到当前帧
		frame.Data.insert(frame.Data.end(),
			mixedData.begin() + currentSampleIndex,
			mixedData.begin() + currentSampleIndex + currentFrameSamples);

		// 计算当前帧的持续时间（单位：100纳秒）
		frame.Duration = static_cast<LONGLONG>((currentFrameSamples * 10000000) / samplesPerSecond);

		// 设置时间戳（可选，如果需要）
		frame.Time = static_cast<LONGLONG>((currentSampleIndex * 10000000) / samplesPerSecond);

		// 添加帧到结果中
		result.push_back(frame);

		// 更新样本索引
		currentSampleIndex += currentFrameSamples;
	}

	return result;
}
/*
将一个主音频和多个连续的副音频按权重进行混合
mainVolume:主音频音量权重[0-1]
secondVolume:副音频音量权重[0-1]
clipToMainAudio:是否将副音频剪切到匹配主音频的长度
loopMode:如果副音频长度比主音频短则循环副音频
*/
std::vector<MixedFrame> AudioMix::MixAudio(double samplesPerSecond, std::vector<MixedFrame> mainAudio, float mainVolume, std::vector<MixedFrame> framesSecond, float secondVolume, bool clipToMainAudio, bool loopMode)
{
	// 计算第一个音频的总样本数
	size_t totalSamplesFirst = 0;
	for (const auto& frame : mainAudio)
	{
		totalSamplesFirst += frame.Data.size();
	}

	// 计算第二个音频的总样本数
	size_t totalSamplesSecond = 0;
	for (const auto& frame : framesSecond)
	{
		totalSamplesSecond += frame.Data.size();
	}

	// 总样本数
	size_t totalSamples = max(totalSamplesFirst, totalSamplesSecond);

	// 创建混合后的样本数据容器
	std::vector<float> mixedData;
	mixedData.reserve(totalSamples);

	// 使用迭代器遍历两个音频的样本数据
	size_t indexFirst = 0, indexSecond = 0;
	size_t frameIndexFirst = 0, frameIndexSecond = 0;
	size_t sampleIndexFirst = 0, sampleIndexSecond = 0;
	float maxVal = 0.0f, minVal = 0.0f;
	while (indexFirst < totalSamplesFirst || indexSecond < totalSamplesSecond)
	{
		float sampleValue = 0.0f;

		// 从第一个音频获取样本值
		if (indexFirst < totalSamplesFirst)
		{
			if (sampleIndexFirst >= mainAudio[frameIndexFirst].Data.size())
			{
				// 移动到下一个帧
				frameIndexFirst++;
				sampleIndexFirst = 0;
			}
			if (frameIndexFirst < mainAudio.size())
			{
				sampleValue += mainAudio[frameIndexFirst].Data[sampleIndexFirst++] * mainVolume;
				indexFirst++;
			}
		}
		if (loopMode && indexSecond >= totalSamplesSecond)
		{
			indexSecond = 0;
			frameIndexSecond = 0;
			sampleIndexSecond = 0;
		}

		// 从第二个音频获取样本值
		if (indexSecond < totalSamplesSecond)
		{
			if (sampleIndexSecond >= framesSecond[frameIndexSecond].Data.size())
			{
				// 移动到下一个帧
				frameIndexSecond++;
				sampleIndexSecond = 0;
			}
			if (frameIndexSecond < framesSecond.size())
			{
				sampleValue += framesSecond[frameIndexSecond].Data[sampleIndexSecond++] * secondVolume;
				indexSecond++;
			}
		}

		if (sampleValue > maxVal)
			maxVal = sampleValue;
		if (sampleValue < minVal)
			minVal = sampleValue;

		// 添加混合后的样本值
		mixedData.push_back(sampleValue);

		if (clipToMainAudio && indexFirst >= totalSamplesFirst)
			break;
	}
	if (maxVal > 0.75f || minVal < -0.75f)
	{
		float maxMul = max(maxVal, -minVal);
		for (auto& fm : mixedData)
		{
			fm /= maxMul;
		}
	}
	// 将混合后的数据分割为帧
	std::vector<MixedFrame> result;
	size_t samplesPerFrame = mainAudio[0].Data.size(); // 每个帧的样本数
	size_t totalMixedSamples = mixedData.size();
	size_t currentSampleIndex = 0;
	size_t currFrameIndex = 0;
	while (currentSampleIndex < totalMixedSamples)
	{
		if (currFrameIndex < mainAudio.size())
			samplesPerFrame = mainAudio[currFrameIndex++].Data.size();
		MixedFrame frame;
		size_t remainingSamples = totalMixedSamples - currentSampleIndex;
		size_t currentFrameSamples = min(samplesPerFrame, remainingSamples);

		// 复制样本数据到当前帧
		frame.Data.insert(frame.Data.end(),
			mixedData.begin() + currentSampleIndex,
			mixedData.begin() + currentSampleIndex + currentFrameSamples);

		// 计算当前帧的持续时间（单位：100纳秒）
		frame.Duration = static_cast<LONGLONG>((currentFrameSamples * 10000000) / samplesPerSecond);

		// 设置时间戳（可选，如果需要）
		frame.Time = static_cast<LONGLONG>((currentSampleIndex * 10000000) / samplesPerSecond);

		// 添加帧到结果中
		result.push_back(frame);

		// 更新样本索引
		currentSampleIndex += currentFrameSamples;
	}

	return result;
}

H264Encoder::H264Encoder(std::wstring destFilename, UINT32 width, UINT32 height, UINT32 fps, UINT32 videoBitrate)
{
	configWMFLib();
	m_VideoBitrate = videoBitrate;
	m_DestFilename = destFilename;
	m_Width = width;
	m_Height = height;
	m_cbWidth = 4 * m_Width;
	m_cbBuffer = m_cbWidth * m_Height;
	m_VideoFPS = fps;
	m_FrameDuration = (10 * 1000 * 1000 / m_VideoFPS);
	MFCreateMemoryBuffer(m_cbBuffer, &m_pBuffer);
	m_pBuffer->SetCurrentLength(m_cbBuffer);
	InitializeWriter();
	m_FrameIndex = 0;
}
H264Encoder::~H264Encoder()
{
	if (this->m_pSinkWriter)
		this->m_pSinkWriter->Release();
	if (this->m_pBuffer)
		m_pBuffer->Release();
	if (this->pVideoSample)
		pVideoSample->Release();
}
HRESULT H264Encoder::AddVideoFrame(IWICBitmap* buffer, LONGLONG sampleTime, LONGLONG duration)
{
	if (pVideoSample == NULL)
	{
		MFCreateSample(&pVideoSample);
		pVideoSample->AddBuffer(m_pBuffer);
	}
	BYTE* pData = NULL;
	HRESULT hr = m_pBuffer->Lock(&pData, NULL, NULL);
	if (SUCCEEDED(hr))
	{
		buffer->CopyPixels(NULL, m_cbWidth, m_cbWidth * m_Height, (BYTE*)pData);
	}
	if (m_pBuffer)
		m_pBuffer->Unlock();
	if (SUCCEEDED(hr))
		hr = pVideoSample->SetSampleTime(sampleTime == 0 ? m_FrameDuration * m_FrameIndex : sampleTime);
	if (SUCCEEDED(hr))
		hr = pVideoSample->SetSampleDuration(duration == 0 ? m_FrameDuration : duration);
	if (SUCCEEDED(hr))
		hr = m_pSinkWriter->WriteSample(VideoStreamIndex, pVideoSample);

	VideoTimeStamp += m_FrameDuration;
	m_FrameIndex += 1;

	return hr;
}
HRESULT H264Encoder::AddVideoFrame(BYTE* buffer, LONGLONG sampleTime, LONGLONG duration)
{
	if (pVideoSample == NULL)
	{
		MFCreateSample(&pVideoSample);
		pVideoSample->AddBuffer(m_pBuffer);
	}
	BYTE* pData = NULL;
	HRESULT hr = m_pBuffer->Lock(&pData, NULL, NULL);
	if (SUCCEEDED(hr))
	{
		RtlCopyMemory(pData, buffer, m_cbWidth * m_Height);
	}
	if (m_pBuffer)
		m_pBuffer->Unlock();
	if (SUCCEEDED(hr))
		hr = pVideoSample->SetSampleTime(sampleTime == 0 ? m_FrameDuration * m_FrameIndex : sampleTime);
	if (SUCCEEDED(hr))
		hr = pVideoSample->SetSampleDuration(duration == 0 ? m_FrameDuration : duration);
	if (SUCCEEDED(hr))
		hr = m_pSinkWriter->WriteSample(VideoStreamIndex, pVideoSample);

	VideoTimeStamp += m_FrameDuration;
	m_FrameIndex += 1;
	return hr;
}
HRESULT H264Encoder::AddAudioFrame(PVOID buffer, DWORD bufferSize, LONGLONG frameDuration)
{
	IMFSample* pSample = NULL;
	IMFMediaBuffer* m_pBuffer = NULL;
	BYTE* pData = NULL;
	HRESULT hr = MFCreateSample(&pSample);
	if (SUCCEEDED(hr))
	{
		hr = MFCreateMemoryBuffer(bufferSize, &m_pBuffer);
		hr = m_pBuffer->SetCurrentLength(bufferSize);
		if (SUCCEEDED(hr))
		{
			hr = m_pBuffer->Lock(&pData, NULL, NULL);
			if (SUCCEEDED(hr))
			{
				memcpy(pData, buffer, bufferSize);
				m_pBuffer->Unlock();
			}
			do {
				hr = pSample->AddBuffer(m_pBuffer);
				BREAK_ON_FAIL(hr);
				hr = pSample->SetSampleTime(this->AudioTimeStamp);
				BREAK_ON_FAIL(hr);
				hr = pSample->SetSampleDuration(frameDuration);
				BREAK_ON_FAIL(hr);
				hr = m_pSinkWriter->WriteSample(AudioStreamIndex, pSample);
				BREAK_ON_FAIL(hr);
				AudioTimeStamp += frameDuration;
			} while (false);
			m_pBuffer->Release();
		}
		pSample->Release();
	}
	return hr;
}
HRESULT H264Encoder::AddAudioFrame(IMFSample* pSample)
{
	DWORD flags = 0;
	HRESULT hr = S_OK;
	do
	{
		if (pSample)
		{
			pSample->GetSampleFlags(&flags);
			pSample->SetSampleTime(this->AudioTimeStamp);
		}
		else
		{
			AudioTimeStamp = VideoTimeStamp;
			return m_pSinkWriter->SendStreamTick(AudioStreamIndex, AudioTimeStamp);
		}
		if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
		{
			hr = m_pSinkWriter->SendStreamTick(AudioStreamIndex, this->GetTimeStamp());
			AudioTimeStamp = this->GetTimeStamp();
			BREAK_ON_FAIL(hr);
			hr = MF_SOURCE_READERF_ENDOFSTREAM;
		}
		else
		{
			hr = m_pSinkWriter->WriteSample(AudioStreamIndex, pSample);
			LONGLONG sampleDuration = 0;
			hr = pSample->GetSampleDuration(&sampleDuration);
			AudioTimeStamp += sampleDuration;
			BREAK_ON_FAIL(hr);
		}
		pSample = NULL;
	} while (false);
	return hr;
}
HRESULT H264Encoder::TickAudioFrame(LONGLONG sampleTime)
{
	this->AudioTimeStamp = sampleTime;
	return m_pSinkWriter->SendStreamTick(AudioStreamIndex, this->AudioTimeStamp);
}
HRESULT H264Encoder::EndAudio()
{
	return m_pSinkWriter->NotifyEndOfSegment(AudioStreamIndex);
}
HRESULT H264Encoder::End()
{
	return m_pSinkWriter->Finalize();
}
HRESULT H264Encoder::ConfigAudioStream()
{
	IMFMediaType* pMediaType = NULL;
	HRESULT hr = MFCreateMediaType(&pMediaType);
	do
	{
		BREAK_ON_NULL(pMediaType, E_POINTER);
		hr = pMediaType->DeleteAllItems();
		BREAK_ON_FAIL(hr);
		pMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		BREAK_ON_FAIL(hr);
		hr = pMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC);
		BREAK_ON_FAIL(hr);
		hr = pMediaType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
		BREAK_ON_FAIL(hr);
		hr = pMediaType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, 44100);
		BREAK_ON_FAIL(hr);
		hr = pMediaType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, 2);
		BREAK_ON_FAIL(hr);
		hr = pMediaType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, 16000);
		BREAK_ON_FAIL(hr);
		hr = pMediaType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, 1);
		BREAK_ON_FAIL(hr);
		hr = m_pSinkWriter->AddStream(pMediaType, &AudioStreamIndex);
		BREAK_ON_FAIL(hr);
		pMediaType->Release();
	} while (false);

	return hr;
}
HRESULT H264Encoder::SetCurrentAudioType(IMFMediaType* mediaType)
{
	return m_pSinkWriter->SetInputMediaType(AudioStreamIndex, mediaType, NULL);
}
HRESULT H264Encoder::InitializeWriter()
{
	HRESULT hr = S_OK;
	m_pSinkWriter = NULL;
	VideoStreamIndex = 0;

	IMFMediaType* pMediaTypeOut = nullptr;
	IMFMediaType* pMediaTypeIn = nullptr;

	do
	{
		hr = MFCreateSinkWriterFromURL(m_DestFilename.c_str(), nullptr, nullptr, &m_pSinkWriter);
		BREAK_ON_FAIL(hr);
		hr = SetVideoOutputType(&pMediaTypeOut);
		BREAK_ON_FAIL(hr);
		hr = SetVideoInputType(&pMediaTypeIn);
		BREAK_ON_FAIL(hr);
		hr = ConfigAudioStream();
		BREAK_ON_FAIL(hr);
	} while (false);

	if (pMediaTypeOut) pMediaTypeOut->Release();
	if (pMediaTypeIn) pMediaTypeIn->Release();
	return hr;
}
HRESULT H264Encoder::Begin()
{
	return m_pSinkWriter->BeginWriting();
}
HRESULT H264Encoder::SetVideoOutputType(IMFMediaType** pMediaTypeOut)
{
	HRESULT hr = S_OK;
	do
	{
		hr = MFCreateMediaType(pMediaTypeOut);
		BREAK_ON_FAIL(hr);
		hr = (*pMediaTypeOut)->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
		BREAK_ON_FAIL(hr);
		hr = (*pMediaTypeOut)->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);
		BREAK_ON_FAIL(hr);
		hr = (*pMediaTypeOut)->SetUINT32(MF_MT_MPEG2_PROFILE, eAVEncH264VProfile_High);
		BREAK_ON_FAIL(hr);

		hr = (*pMediaTypeOut)->SetUINT32(MF_MT_AVG_BITRATE, m_VideoBitrate);
		BREAK_ON_FAIL(hr);
		hr = (*pMediaTypeOut)->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
		BREAK_ON_FAIL(hr);
		hr = MFSetAttributeSize((*pMediaTypeOut), MF_MT_FRAME_SIZE, m_Width, m_Height);
		BREAK_ON_FAIL(hr);
		hr = MFSetAttributeRatio((*pMediaTypeOut), MF_MT_FRAME_RATE, m_VideoFPS, 1);
		BREAK_ON_FAIL(hr);
		hr = MFSetAttributeRatio((*pMediaTypeOut), MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
		BREAK_ON_FAIL(hr);
		hr = m_pSinkWriter->AddStream(*pMediaTypeOut, &VideoStreamIndex);
	} while (false);
	return hr;
}
HRESULT H264Encoder::SetVideoInputType(IMFMediaType** pMediaTypeIn)
{
	HRESULT hr = S_OK;
	do
	{
		hr = MFCreateMediaType(pMediaTypeIn);
		BREAK_ON_FAIL(hr);
		hr = (*pMediaTypeIn)->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
		BREAK_ON_FAIL(hr);
		hr = (*pMediaTypeIn)->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
		BREAK_ON_FAIL(hr);
		hr = (*pMediaTypeIn)->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
		BREAK_ON_FAIL(hr);
		hr = MFSetAttributeSize(*pMediaTypeIn, MF_MT_FRAME_SIZE, m_Width, m_Height);
		BREAK_ON_FAIL(hr);
		hr = MFSetAttributeRatio(*pMediaTypeIn, MF_MT_FRAME_RATE, m_VideoFPS, 1);
		BREAK_ON_FAIL(hr);
		hr = MFSetAttributeRatio(*pMediaTypeIn, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
		BREAK_ON_FAIL(hr);
		hr = m_pSinkWriter->SetInputMediaType(VideoStreamIndex, *pMediaTypeIn, NULL);
	} while (false);

	return hr;
}
HRESULT H264Encoder::InitTranscodeVideoType(CComPtr<IMFMediaType>& pStreamMediaType)
{
	HRESULT hr = S_OK;

	do
	{
		BREAK_ON_NULL(pStreamMediaType, E_POINTER);
		hr = pStreamMediaType->DeleteAllItems();
		BREAK_ON_FAIL(hr);
		pStreamMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
		BREAK_ON_FAIL(hr);
		hr = pStreamMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);

		BREAK_ON_FAIL(hr);

		hr = MFSetAttributeSize(
			pStreamMediaType, MF_MT_FRAME_SIZE, m_Width, m_Height);
		BREAK_ON_FAIL(hr);

		hr = MFSetAttributeRatio(
			pStreamMediaType, MF_MT_FRAME_RATE, m_VideoFPS, 1);
		BREAK_ON_FAIL(hr);

		hr = pStreamMediaType->SetUINT32(MF_MT_AVG_BITRATE, m_VideoBitrate);
		BREAK_ON_FAIL(hr);

		hr = pStreamMediaType->SetUINT32(MF_MT_INTERLACE_MODE,
			MFVideoInterlace_Progressive);
		BREAK_ON_FAIL(hr);

		hr = MFSetAttributeSize(pStreamMediaType, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
		BREAK_ON_FAIL(hr);
	} while (false);

	return hr;
}
double H264Encoder::CurrentTimeOfSeconds()
{
	return double(m_FrameIndex) / double(m_VideoFPS);
}
UINT64 H264Encoder::GetTimeStamp()
{
	return m_FrameDuration * m_FrameIndex;
}
UINT64 H264Encoder::GetTimeStampForIndex(UINT32 index)
{
	return m_FrameDuration * index;
}

AudioCapture::AudioCapture()
{
	configWMFLib();
	HRESULT hr = CoCreateInstance(
		__uuidof(MMDeviceEnumerator),
		NULL, CLSCTX_ALL,
		__uuidof(IMMDeviceEnumerator),
		(void**)&pEnumerator
	);
	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	hr = pDevice->Activate(
		__uuidof(IAudioClient),
		CLSCTX_ALL,
		NULL,
		(void**)&pAudioClient
	);

	hr = pAudioClient->GetMixFormat(&pwfx);
	hr = pAudioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED,
		AUDCLNT_STREAMFLAGS_LOOPBACK,
		0, 0, pwfx, NULL
	);
	hr = pAudioClient->GetService(
		__uuidof(IAudioCaptureClient),
		(void**)&pCaptureClient
	);
}
AudioCapture::~AudioCapture()
{
	CoTaskMemFree(pwfx);
	pCaptureClient->Release();
	pAudioClient->Release();
	pDevice->Release();
	pEnumerator->Release();
}
HRESULT AudioCapture::Start()
{
	return pAudioClient->Start();
}
HRESULT AudioCapture::Stop()
{
	return pAudioClient->Stop();
}
std::vector<uint8_t> AudioCapture::Capture(UINT32* packetLength, UINT32* numFramesAvailable, DWORD* dwFlags)
{
	BYTE* pData = NULL;
	std::vector<uint8_t> result;
	if (SUCCEEDED(pCaptureClient->GetNextPacketSize(packetLength)) && *packetLength > 0)
	{
		HRESULT hr = pCaptureClient->GetBuffer(&pData, numFramesAvailable, dwFlags, NULL, NULL);
		if (SUCCEEDED(hr))
		{
			UINT32 bufferSize = *numFramesAvailable * pwfx->nBlockAlign;
			if (bufferSize == 0)
			{
				pCaptureClient->ReleaseBuffer(*numFramesAvailable);
				return result;
			}
			result.resize(bufferSize);
			memcpy(result.data(), pData, bufferSize);
			pCaptureClient->ReleaseBuffer(*numFramesAvailable);
		}
	}
	return result;
}
HRESULT AudioCapture::Capture(BYTE* buffer, UINT32* bufferSize, LONGLONG* duration)
{
	HRESULT hr = S_OK;
	BYTE* pData = NULL;
	UINT32 packetLength;
	UINT32 numFramesAvailable;
	DWORD dwFlags;
	*bufferSize = 0;
	*duration = 0;

	hr = pCaptureClient->GetNextPacketSize(&packetLength);
	if (FAILED(hr))
	{
		return hr;
	}
	if (packetLength <= 0)
	{
		return AUDCLNT_S_BUFFER_EMPTY;
	}

	hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &dwFlags, NULL, NULL);
	if (FAILED(hr) || hr == AUDCLNT_S_BUFFER_EMPTY)
	{
		return hr;
	}
	*bufferSize = numFramesAvailable * pwfx->nBlockAlign;
	if (*bufferSize == 0)
	{
		pCaptureClient->ReleaseBuffer(numFramesAvailable);
		return AUDCLNT_S_BUFFER_EMPTY;
	}
	memcpy(buffer, pData, *bufferSize);
	*duration = ((double)numFramesAvailable / (double)pwfx->nSamplesPerSec) * 10000000.0;
	pCaptureClient->ReleaseBuffer(numFramesAvailable);
	return S_OK;
}
UINT32 AudioCapture::BytesPerFrame()
{
	return pwfx->nBlockAlign;
}
DWORD AudioCapture::Channels()
{
	return pwfx->nChannels;
}
DWORD AudioCapture::SamplesPerSecond()
{
	return pwfx->nSamplesPerSec;
}
IMFMediaType* AudioCapture::GetMediaType()
{
	HRESULT hr = S_OK;
	IMFMediaType* pMediaTypeOut = NULL;
	hr = MFCreateMediaType(&pMediaTypeOut);
	if (SUCCEEDED(hr)) {
		hr = pMediaTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	}

	if (SUCCEEDED(hr)) {
		hr = pMediaTypeOut->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float);
	}

	if (SUCCEEDED(hr)) {
		hr = pMediaTypeOut->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, pwfx->nChannels);
	}

	if (SUCCEEDED(hr)) {
		hr = pMediaTypeOut->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, pwfx->nSamplesPerSec);
	}

	if (SUCCEEDED(hr)) {
		hr = pMediaTypeOut->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, pwfx->nBlockAlign);
	}

	if (SUCCEEDED(hr)) {
		hr = pMediaTypeOut->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, pwfx->nAvgBytesPerSec);
	}

	if (SUCCEEDED(hr)) {
		hr = pMediaTypeOut->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, pwfx->wBitsPerSample);
	}

	if (SUCCEEDED(hr)) {
		hr = pMediaTypeOut->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
	}
	return pMediaTypeOut;
}