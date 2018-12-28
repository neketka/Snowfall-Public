#pragma once

class JobHandle
{
public:
	void Start();
	void Lock();
	bool IsComplete();
	void Then(JobHandle handle);
};

class JobManager
{
public:
	JobManager();
	~JobManager();
};

