///YS URL±àÂë½âÂëº¯Êý
#include <stdio.h>
#include <string.h>

#define LEN 128

int hexToInt (char c)
{
    if (c == 'A' || c == 'a')
        return 10;
    else if (c == 'B' || c == 'b')
        return 11;
    else if (c == 'C' || c == 'c')
        return 12;
    else if (c == 'D' || c == 'd')
        return 13;
    else if (c == 'E' || c == 'e')
        return 14;
    else if (c == 'F' || c == 'f')
        return 15;
    return (c-'0');
}



int uri_iconv (char *args)
{
    if (strchr(args, '%') == NULL    ) {

    /*    p2=fopen(args,"w+");
        fwrite(args,strlen(args),1,p2);
        fclose(p2);
   */     return 1;

    }

    char buf[LEN] = {'\0'};
    char *p = args;
    int i = 0;
L1:
    while (*p &&( *p != '%'  ))
        *(buf + i++) = *p++;
    if (! *p)
        goto L2;
    p++;
    *(buf + i++) = (char)(hexToInt(*p++) * 16 + hexToInt(*p++));    goto L1;
L2:
    *(buf + i) = '\0';

 strcpy(args,buf);
 //my_iconv(args);
   /*     p2=fopen(buf,"w+");
        fwrite(buf,strlen(buf),1,p2);
        fclose(p2);
*/
    return 1;

}



int email_content_iconv (char *args)
{
    if (strchr(args, '%') == NULL    ) {
   return 1;
    }

    char buf[1024*128] = {'\0'};
    char *p = args;
    int i = 0;
L1:
    while (*p &&( *p != '%'  ))
        *(buf + i++) = *p++;
    if (! *p)
        goto L2;
    p++;
    *(buf + i++) = (char)(hexToInt(*p++) * 16 + hexToInt(*p++));    goto L1;
L2:
    *(buf + i) = '\0';

 strcpy(args,buf);

    return 1;

}
