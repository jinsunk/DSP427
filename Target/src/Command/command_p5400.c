#include <string.h>		// memset, strtok
#include <stdlib.h>		// malloc

#include "command.h"
//#include "memset.h"

void *pDynamicCmdVariable;
LIST lCommand;	//

PCOMMAND FindCommand(char *strcmd)
{

	PNODE pNode = lCommand.m_pNodeHead;
	PCOMMAND pCommand;

	while( pNode )
	{
		pCommand = (PCOMMAND)pNode->pObject;

		if ( strcmp(pCommand->string, strcmd) == 0 )
		{
			// 찾았을 때
			return pCommand;
		}
		else
			pNode = pNode->pNext;
	}

	return (void*)0;	// 못 찾았을 때
}

int AddCommand(PLIST pList, PCOMMAND pCommand)
{
	PNODE pNewNode;

	// NODE를 만든다
	pNewNode = (PNODE)malloc(sizeof(NODE));

	if ( pNewNode )
	{
		pNewNode->pObject = pCommand;
		ListAddNodeTail(pList, pNewNode);
	}

	return 0;
}

void InitCommand(void)
{
	// 사용할 인라인 명령어를 등록한다
	ListInit(&lCommand, ASCENT | LINEAR);

	// 사용할 COMMAND를 덧 붙인다
	AddCommand(&lCommand, &cmd_RealAI);
//	AddCommand(&lCommand, &cmd_Buff);
	AddCommand(&lCommand, &cmd_Debug_H);
	AddCommand(&lCommand, &cmd_Debug_help);
	//AddCommand(&lCommand, &cmd_Debug_DFTV);

	AddCommand(&lCommand, &cmd_Debug_Read);

	AddCommand(&lCommand, &cmd_Debug_MODBUS);
	AddCommand(&lCommand, &cmd_readblk);
	AddCommand(&lCommand, &cmd_eraseblk);
	AddCommand(&lCommand, &cmd_Debug_TEST);
	AddCommand(&lCommand, &cmd_Debug_CALIB);

	AddCommand(&lCommand, &cmd_Event);

}