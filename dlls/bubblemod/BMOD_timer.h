class BmodTimer
{
	cvar_t* m_pmp_timelimit;
	float m_fNextTimerUpdate;
	float m_fLastTimeCheck;
	float m_fEffectiveTime;

public:
	BmodTimer();
	virtual ~BmodTimer();

	void Think();
};
