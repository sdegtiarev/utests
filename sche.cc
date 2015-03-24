#include <unistd.h>
#include <sched.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <sstream>
#include <itc/error.h>
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
	unsigned cnt;
	bool verbose=0;
	sync_sched() : cnt(0) {}
};

void test(int policy, sched_param const& priority, sync_sched& s) noexcept
try {
	using lock=std::unique_lock<std::mutex>;
	lock guard{s.m};
	sched(policy, priority);
	auto id=++s.cnt;
	if(s.verbose)
		std::cout<<id<<": "<<policy_tag(policy, priority)<<" is ready"<<std::endl;
	s.cv.wait(guard);
	std::cout<<id<<": "<<policy_tag(policy, priority)<<" is here"<<std::endl;
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
	while((opt=getopt(argc,argv,"d:b:i:f:r:vh")) != -1) {
		++cnt;
		int policy=-1;
		switch(opt) {
			case 'd': policy=SCHED_OTHER; break;
			case 'b': policy=SCHED_BATCH; break;
			case 'i': policy=SCHED_IDLE; break;
			case 'f': policy=SCHED_FIFO; break;
			case 'r': policy=SCHED_RR; break;
			case 'v': sync.verbose=1; break;
			case 'h':
			default : usage(); return 1;
		}
		if(optarg) {
			++cnt;
			sched_param priority{atoi(optarg)};
			std::thread th([policy,priority,&sync]{ test(policy, priority, sync); });
			th.detach();
		}
	}
	
	while(sync.cnt < cnt) { usleep(1); }
	if(sync.verbose)
		std::cout<<std::string(60,'-')<<std::endl;
	sync.cv.notify_all();
	while(sync.cnt > 0) { usleep(1); }
	if(sync.verbose)
		std::cout<<std::string(60,'-')<<std::endl;
		
	
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
	"  -v	- verbose, print the order tasks initialized\n"
	"  -h	- print this help and quit\n"
	;	
}
