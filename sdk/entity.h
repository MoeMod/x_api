typedef struct entbind_s
{
	int size;
	void *data;
}
entbind_t;

extern entbind_t *entbinds;

void AllocEntityData(int entity, int structs);
void FreeEntityData(int entity);
void WriteEntityData(int entity, int offset, int value);
int ReadEntityData(int entity, int offset);
void WriteEntityString(int entity, int offset, char *string, int len);
void ReadEntityString(int entity, int offset, char *string, int len);