#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>

#define MAX_LINES 8
#define LINE_LEN  512
#define FIELD_LEN 256

static void GetDistro(char *out, size_t size){
    FILE *file = fopen("/etc/os-release", "r");
    snprintf(out, size, "Unknown");
	
    if(!file){
		return;
	}

    char line[256];
    while(fgets(line, sizeof line, file)){
        if(strncmp(line, "PRETTY_NAME=", 12) == 0){
            char *val = line + 12;
            val[strcspn(val, "\r\n")] = '\0';
            size_t len = strlen(val);
            
            if(len >= 2 && (val[0] == '"' || val[0] == '\'') && val[len - 1] == val[0]){
                val[len - 1] = '\0';
                val++;
            }
			
            snprintf(out, size, "%s", val);
            break;
        }
    }
	
    fclose(file);
}

static void GetUptime(char *out, size_t size){
    struct sysinfo systemInfo;
	
    if(sysinfo(&systemInfo) != 0){
        snprintf(out, size, "unknown");
        return;
    }
	
    long up = systemInfo.uptime;
    long days = up / 86400;
    long hours = (up % 86400) / 3600;
    long mins = (up % 3600) / 60;

    if(days > 0){
        snprintf(out, size, "%ldd %ldh %ldm", days, hours, mins);
	}
    else if(hours > 0){
		snprintf(out, size, "%ldh %ldm", hours, mins);
	}
    else{
		snprintf(out, size, "%ldm", mins);
	}
}

static void GetKernel(char *out, size_t size){
    struct utsname machine;
	
    if(uname(&machine) != 0){
		snprintf(out, size, "unknown");
	} 
    else{
        snprintf(out, size, "%s %s", machine.sysname, machine.release);		
	}
}

static void GetDate(char *out, size_t size){
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
	
    if(!tm || strftime(out, size, "%a %d %b %Y, %H:%M", tm) == 0){
        snprintf(out, size, "unknown");		
	}
}

static void PrintTextBubble(char lines[][LINE_LEN], int n){
    size_t width = 0;
	
    for (int i = 0; i < n; i++){
        size_t len = strlen(lines[i]);
        if(len > width) width = len;
    }

    printf(" ");
    for (size_t i = 0; i < width + 2; i++){
		putchar('_');
	}
    printf("\n");

    for (int i = 0; i < n; i++){
        const char *left, *right;
		
        if(n == 1){
			left = "<"; right = ">";
		}
        else if(i == 0){
			left = "/"; right = "\\";
		}
        else if(i == n - 1){
			left = "\\"; right = "/";
		}
        else{
			left = "|"; right = "|";
		}

        printf("%s %-*s %s\n", left, (int)width, lines[i], right);
    }

    printf(" ");
    for (size_t i = 0; i < width + 2; i++) putchar('-');
    printf("\n");
}

static void PrintCow(void){
    printf("        \\   ^__^\n"
           "         \\  (oo)\\_______\n"
           "            (__)\\       )\\/\\\n"
           "                ||----w |\n"
           "                ||     ||\n");
}

int main(void){
    char lines[MAX_LINES][LINE_LEN];
    char distro[FIELD_LEN], uptime[FIELD_LEN], kernel[FIELD_LEN], date[FIELD_LEN];

    GetDistro(distro, sizeof distro);
    GetUptime(uptime, sizeof uptime);
    GetKernel(kernel, sizeof kernel);
    GetDate(date, sizeof date);

    snprintf(lines[0], LINE_LEN, "distribution: %s", distro);
    snprintf(lines[1], LINE_LEN, "uptime:       %s", uptime);
    snprintf(lines[2], LINE_LEN, "kernel:       %s", kernel);
    snprintf(lines[3], LINE_LEN, "date:         %s", date);

    PrintTextBubble(lines, 4);
    PrintCow();
    return 0;
}
