extern int gmsgTeamScore;
extern void *(*g_pfnInstallGameRules)(void);
extern JMPHook *g_phInstallGameRules;

void *InstallGameRules(void);
void PatchRoundEnd(int noend);
void TerminateRound(float tmDelay, int iWinStatus);
int GetTeamScore(int team);
void SetTeamScore(int team, int score);
char *GetTeam(int team);
void UpdateTeamScore(int team);