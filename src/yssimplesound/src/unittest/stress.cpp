#include <cstdio>
#include <vector>

#include <fssimplewindow.h>
#include <yssimplesound.h>


const int samplePerChannel=44100;


void MakeSilence(std::vector <unsigned char> &wave)
{
	wave.resize(samplePerChannel*4);  // 4-bytes per sample * 44.1KHz = 1 second
	memset((char *)wave.data(),0,wave.size());
}

bool RecyclePlayer(void)
{
	std::vector <unsigned char> wave;
	MakeSilence(wave);

	YsSoundPlayer::SoundData data;
	data.CreateFromSigned16bitStereo(44100,wave);

	if(data.GetNumSamplePerChannel()!=samplePerChannel)
	{
		fprintf(stderr,"Sample count error.\n");
		return false;
	}

	YsSoundPlayer sndPlayer;
	for(int i=0; i<10; ++i)
	{
		printf("%d\n",i);
		sndPlayer.Start();
		sndPlayer.PlayOneShot(data);
		while(YSTRUE==sndPlayer.IsPlaying(data))
		{
			FsPollDevice();
			sndPlayer.KeepPlaying();
		}
		sndPlayer.End();
	}
	return true;
}

int main(void)
{
	FsOpenWindow(0,0,100,100,0);

	if(true!=RecyclePlayer())
	{
		fprintf(stderr,"Player Recycling failed.\n");
		return 1;
	}



	printf("End Test.\n");
	return 0;
}
