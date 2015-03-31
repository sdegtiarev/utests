#include <unistd.h>
#include <sched.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <sstream>
#include <error.h>
static void usage();


std::string policy_tag(int policy, sched_param const& priority)
{
	std::ostringstream tag;
	switch(policy) {
		case SCHED_OTHER: tag<<"SCHED_OTHER["<<priority.__sched_priority<<"]"; return tag.str();
		case SCHED_BATCH: tag<<"SCHED_BATCH["<<priority.__sched_priority<<"]"; return tag.str();
		case SCHED_IDLE: tag<<"SCHED_IDLE["<<priority.__sched_priority<<"]"; return tag.str();
		case SCHED_FIFO: tag<<"SCHED_FIFO["<<priority.__sched_priority<<"]"; return tag.str();
		case SCHED_RR: tag<<"SCHED_RR["<<priority.__sched_priority<<"]"; return tag.str();
	}
	tag<<"SCHED_INVALID["<<priority.__sched_priority<<"]";
	return tag.str();
}



void sched(int policy, sched_param const& priority)
{
	if(sched_setscheduler(0, policy, &priority))
		throw itc::syserr(policy_tag(policy, priority));
}


struct sync_sched
{
	std::mutex m;
	std::condition_variable cv;
	unsigned cnt	=0;
};

void test(int policy, sched_param const& priority, sync_sched& s) noexcept
try {
	using lock=std::unique_lock<std::mutex>;
	lock guard{s.m};
	sched(policy, priority);
	auto id=++s.cnt;
	s.cv.wait(guard);
	std::cout<<id<<" "<<policy_tag(policy, priority)<<std::endl;
	--s.cnt;

} catch(std::exception const &x) {
	std::cerr<<x.what()<<std::endl;
	exit(1);
} catch(...) {
	std::cerr<<"unexpected exception in thread"<<std::endl;
	exit(1);
}




main(int argc, char** argv)
try {
	sync_sched sync;
	int cnt=0;
	int opt;
	while((opt=getopt(argc,argv,"d:b:i:f:r:h")) != -1) {
		++cnt;
		int policy=-1;
		switch(opt) {
			case 'd': policy=SCHED_OTHER; break;
			case 'b': policy=SCHED_BATCH; break;
			case 'i': policy=SCHED_IDLE; break;
			case 'f': policy=SCHED_FIFO; break;
			case 'r': policy=SCHED_RR; break;
			case 'h':
			default : usage(); return 1;
		}
		
		
		sched_param priority{atoi(optarg)};
		auto x=sync.cnt;
		std::thread th([policy,priority,&sync]{ test(policy, priority, sync); });
		th.detach();
		while(x == sync.cnt)
			usleep(1);
	}
	if(optind >= argc) { usage(); return 1; }

	
	while(sync.cnt < cnt) { usleep(1); }
	sync.cv.notify_all();
	while(sync.cnt > 0) { usleep(1); }
	
	return 0;
} catch(std::exception const& x) {
	std::cerr<<x.what()<<std::endl;
	return 1;
}

static void usage()
{
	std::cout<<"sche: testing scheduler policies\n"
	"Syntax: sche { -d|b|i|f|r <priority>}... [-h]\n"
	"Options:\n"
	"  -d	- SCHED_OTHER, default scheduler policy\n"
	"  -b	- SCHED_BATCH, lower priority for batch processes\n"
	"  -i	- SCHED_IDLE, very low priority for background tasks\n"
	"  -f	- SCHED_FIFO, high priority with no thread yield\n"
	"  -r	- SCHED_RR, high priority with round robin\n"
	"  -h	- print this help and quit\n"
	"Note that for normal operation SCHED_OTHER, SCHED_BATCH and\n"
	"  SCHED_IDLE must have priority 0, while SCHED_FIFO and\n"
	"  SCHED_RR must be in range ["
	<<sched_get_priority_min(SCHED_FIFO)<<", "
	<<sched_get_priority_max(SCHED_FIFO)<<"]"
	" and under root permission\n"
	;
}
