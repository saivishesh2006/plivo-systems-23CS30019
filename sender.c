#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void) {
    int in_fd = socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in in_addr = {0};
    in_addr.sin_family = AF_INET;
    in_addr.sin_port = htons(47010);
    in_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (bind(in_fd,(struct sockaddr*)&in_addr,sizeof in_addr) < 0) {
        perror("bind 47010");
        return 1;
    }

    int out_fd = socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in relay = {0};
    relay.sin_family = AF_INET;
    relay.sin_port = htons(47001);
    relay.sin_addr.s_addr = inet_addr("127.0.0.1");

    unsigned char buf[2048];
    
    for (;;) {
        ssize_t n = recvfrom(in_fd, buf, sizeof buf, 0, NULL, NULL);
        if (n < 164) continue;
        
        uint32_t seq32;
        memcpy(&seq32, buf, 4);
        seq32 = ntohl(seq32);
        
        //custom wire format: 2 byte seq + 160 byte payload = 162 bytes
        //this saves 2 bytes per frame, allowing us more headroom for duplicates
        unsigned char out_buf[162];
        uint16_t seq16 = htons((uint16_t)seq32);
        memcpy(out_buf, &seq16, 2);
        memcpy(out_buf + 2, buf + 4, 160);
        
        //send first copy
        sendto(out_fd,out_buf,162,0,(struct sockaddr *)&relay,sizeof relay);
        
        //to achieve minimum possible DELAY_MS,we want to avoid relying on FEC
        //piggybacked on future frames(which introduces a 20ms delay).
        //instead,we immediately send a duplicate of the current frame for 95% of frames!
        //this provides near-instant recovery from random drops.
        //the bandwidth overhead is exactly:(19*324+162)/(20*160) = 1.974x(under 2.0x limit).
        if ((seq32%20)!=0) {
            sendto(out_fd,out_buf,162,0,(struct sockaddr *)&relay,sizeof relay);
        }
    }
    return 0;
}
