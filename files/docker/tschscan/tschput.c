#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE
#include <stdio.h>

int main(int argc,char *argv[]) {
	FILE *fp;
	unsigned char buf[2048],*pbuf,lbuf[188],sbuf[4096],*spnc,*snc;
	int i,j,k,f;
	int s,p,dc,lcc[8192],df;
	int pusi,pid,afc,cc,afl,di,pcrf;
	int sp,pp,ps,pf,cvn,csn,conid,ctsid;
	int tid,ssi,sl,tie,vn,cni,sn,lsn;
	int tsid,onid,sid,eudf,esf,epff,rs,fcm,dll;
	int dt,dl,st,spnl,snl;
	int g[4],gl,gr,ss,jc,jh,jl;
	unsigned long long ac;

	fp=fopen(argv[1],"rb");
	s=2048;
	p=2048;
	dc=0;
	lbuf[0]=0x00;
	for(i=0;i<8192;i++) lcc[i]=-1;
	sp=-1;
	cvn=-1;
	conid=-1;
	ctsid=-1;
	csn=0;
	f=0;
	while(1) {
		/* パケットを読み込む */
		if((s-p)>=188) p=p+188;
		while(buf[p]!=0x47 || (s-p)<188) {
			while(buf[p]!=0x47 && p<s) p++;
			if((s-p)<188) {
				if(s<2048) break;
				for(i=p;i<s;i++) buf[i-p]=buf[i];
				s=(s-p)+fread(buf+(s-p),1,2048-(s-p),fp);
				p=0;
			}
		}
		pbuf=buf+p;
		if((s-p)<188) break;

		/* パケット内容を取得する */
		pusi=(pbuf[1]&0x40)>>6;
		pid=((pbuf[1]<<8)|pbuf[2])&0x1fff;
		afc=(pbuf[3]&0x30)>>4;
		cc=pbuf[3]&0x0f;
		afl=-1;
		if((afc&0x02)!=0) afl=pbuf[4];
		di=-1;
		pcrf=-1;
		if(afl>=1) {
			di=(pbuf[5]&0x80)>>7;
			pcrf=(pbuf[5]&0x10)>>4;
		}

		/* 重複パケットを確認する */
		if(pcrf==1 && afl>=7) for(i=0;i<6;i++) lbuf[6+i]=buf[6+i];
		for(i=0;i<188;i++) if(pbuf[i]!=lbuf[i]) break;
		if(i<188) dc=0;
		else dc=dc+1;
		if(dc>1) dc=0;
		if((afc&0x01)==0 || pid==0x1fff) dc=0;
		for(i=0;i<188;i++) lbuf[i]=pbuf[i];

		/* ドロップを確認する */
		df=0;
		if(pid!=0x1fff) {
			if(lcc[pid]>=0 && di!=1) {
				if((afc&0x01)!=0 && dc==0) {
					if(cc!=((lcc[pid]+1)&0x0f)) df=1;
				}
				else {
					if(cc!=lcc[pid]) df=1;
				}
			}
			lcc[pid]=cc;
		}

		/* セクションを取得する */
		if(pid==0x0011 && (afc&0x01)!=0 && df==1) sp=-1;
		while(pid==0x0011 && (afc&0x01)!=0 && (pusi==1 || sp>=0) && dc==0) {
			pp=4;
			ps=188;
			if(afl>=0) pp=pp+1+afl;
			if(pusi==1) {
				pf=pbuf[pp];
				pp++;
				if(sp<0) {
					/* セクションの先頭の場合 */
					pp=pp+pf;
					for(i=0;i<(ps-pp);i++) sbuf[i]=pbuf[pp+i];
					sp=ps-pp;
				}
				else {
					/* セクションの末尾の場合 */
					ps=pp+pf;
					if(ps>188) ps=188;
					for(i=0;(i<(ps-pp) && (sp+i)<4096);i++) sbuf[sp+i]=pbuf[pp+i];
					sp=-1;
				}
			}
			else {
				/* セクションの中間の場合 */
				for(i=0;(i<(ps-pp) && (sp+i)<4096);i++) sbuf[sp+i]=pbuf[pp+i];
				sp=sp+(ps-pp);
			}
			if(sp>=3) {
				sl=((sbuf[1]<<8)|sbuf[2])&0x0fff;
				if(sp>=(sl+3)) sp=-1;
			}

			if(sp<0) {
				/* セクション内容を取得する */
				tid=sbuf[0];
				ssi=(sbuf[1]&0x80)>>7;
				sl=((sbuf[1]<<8)|sbuf[2])&0x0fff;
				tie=-1;
				vn=-1;
				cni=-1;
				sn=-1;
				lsn=-1;
				if(ssi==1 && sl>=5) {
					tie=(sbuf[3]<<8)|sbuf[4];
					vn=(sbuf[5]&0x3e)>>1;
					cni=sbuf[5]&0x01;
					sn=sbuf[6];
					lsn=sbuf[7];
				}

				/* サービス記述テーブルの内容を取得する */
				tsid=tie;
				onid=(sbuf[8]<<8)|sbuf[9];
				if(tid==0x42 && conid<0) conid=onid;
				if(tid==0x42 && ctsid<0) ctsid=tsid;
				if(tid==0x42 & sl>=(5+3+5+4) && vn!=cvn && cni==1 && sn==csn && onid==conid && tsid==ctsid) {
					cvn=vn;
					i=0;
					while(i<(sl-5-4-3)) {
						sid=(sbuf[11+i]<<8)|sbuf[12+i];
						eudf=(sbuf[13+i]&0x1c)>>2;
						esf=(sbuf[13+i]&0x02)>>1;
						epff=sbuf[13+i]&0x01;
						rs=(sbuf[14+i]&0xe0)>>5;
						fcm=(sbuf[14+i]&0x10)>>4;
						dll=((sbuf[14+i]<<8)|sbuf[15+i])&0x0fff;

						/* 記述子領域の内容を取得する */
						j=0;
						st=-1;
						spnl=-1;
						spnc=NULL;
						snl=-1;
						snc=NULL;
						while(rs==0 && j<dll) {
							/* 記述子の内容を取得する */
							dt=sbuf[16+i+j];
							dl=sbuf[17+i+j];
							if(dt==0x48) {
								/* サービス記述子の内容を取得する */
								st=sbuf[18+i+j];
								spnl=sbuf[19+i+j];
								spnc=sbuf+20+i+j;
								snl=sbuf[20+i+j+spnl];
								snc=sbuf+21+i+j+spnl;
							}
							j=j+2+dl;
						}
						i=i+5+dll;

						/* デジタルTVサービスなら内容を表示する */
						if(st==1) {
							f=1;
							printf("%1d %5d %5d %5d ",fcm,onid,tsid,sid);
							/* for(k=0;k<snl;k++) printf("%02X ",snc[k]); */

							/* ARIB 8単位符号をシフトJISに変換して出力する */
							g[0]=0x0142;	/* G0=漢字(2バイトGセット) */
							g[1]=0x004a;	/* G1=英数(1バイトGセット) */
							g[2]=0x0030;	/* G2=平仮名(1バイトGセット) */
							g[3]=0x0031;	/* G3=片仮名(1バイトGセット) */
							gl=0;		/* GL=G0 */
							gr=2;		/* GR=G2 */
							ss=-1;		/* シングルシフト解除 */
							ac=0;
							k=0;
							while(k<snl) {
								jc=0;
								ac=(ac<<8)|snc[k];
								k++;
								if(ac==0x0e) {				/* LS1 (GL=G1) */
									gl=1;
									ac=0;
								}
								else if(ac==0x0f) {			/* LS0 (GL=G0) */
									gl=0;
									ac=0;
								}
								else if(ac==0x19) {			/* SS2 (GL=G2 シングルシフト) */
									ss=gl;
									gl=2;
									ac=0;
								}
								else if(ac==0x1d) {			/* SS3 (GL=G3 シングルシフト) */
									ss=gl;
									gl=3;
									ac=0;
								}
								else if(ac==0x1b6e) {			/* LS2 (GL=G2) */
									gl=2;
									ac=0;
								}
								else if(ac==0x1b6f) {			/* LS3 (GL=G3) */
									gl=3;
									ac=0;
								}
								else if(ac==0x1b7c) {			/* LS3R (GR=G3) */
									gr=3;
									ac=0;
								}
								else if(ac==0x1b7d) {			/* LS2R (GR=G2) */
									gr=2;
									ac=0;
								}
								else if(ac==0x1b7e) {			/* LS1R (GR=G1) */
									gr=1;
									ac=0;
								}
								else if(ac==0x89) {			/* MSZ (中型サイズ) */
									ac=0;
								}
								else if(ac>=0x21 && ac<=0x7e) {		/* GL (1バイト文字) */
									if(g[gl]==0x004a) jc=0x2300+ac;		/* 英数 */
									else if(g[gl]==0x0030) jc=0x2400+ac;	/* 平仮名 */
									else if(g[gl]==0x0031) jc=0x2500+ac;	/* 片仮名 */
									if((g[gl]&0x0f00)==0x0000) ac=0;
								}
								else if(ac>=0x2100 && ac<=0x7eff) {	/* GL (2バイト文字) */
									if((ac&0xff)>=0x21 && (ac&0xff)<=0x7e && g[gl]==0x0142) jc=ac;
									ac=0;
								}
								else if(ac>=0xa1 && ac<=0xfe) {		/* GR (1バイト文字) */
									if(g[gr]==0x004a) jc=0x2300+(ac&0x7f);		/* 英数 */
									else if(g[gr]==0x0030) jc=0x2400+(ac&0x7f);	/* 平仮名 */
									else if(g[gr]==0x0031) jc=0x2500+(ac&0x7f);	/* 片仮名 */
									if((g[gr]&0x0f00)==0x0000) ac=0;
								}
								else if(ac>=0xa100 && ac<=0xfeff) {	/* GR (2バイト文字) */
									if((ac&0xff)>=0xa1 && (ac&0xff)<=0xfe && g[gr]==0x0142) jc=ac&0x7f7f;
									ac=0;
								}
								if(jc>0) {
									if(jc==0x2321) jc=0x212a;	/* エクスクラメーション */
									else if(jc==0x232d) jc=0x215d;	/* マイナス */
									else if(jc==0x232f) jc=0x213f;	/* スラッシュ */
									else if(jc==0x247e) jc=0x2126;	/* 中黒記号(平仮名) */
									else if(jc==0x2579) jc=0x213c;	/* 長音記号(片仮名) */
									else if(jc==0x257e) jc=0x2126;	/* 中黒記号(平仮名) */

									jh=(((jc/256)-0x21)/2)+0x81;
									if(jh>=0xa0) jh=jh+0x40;
									if(((jc/256)%2)==0) {
										jl=(jc%256)-0x21+0x9f;
									}
									else {
										jl=(jc%256)-0x21+0x40;
										if(jl>=0x7f) jl=jl+0x01;
										}
									printf("%c%c",jh,jl);

									if(ss>=0) {			/* シングルシフト解除 */
										gl=ss;
										ss=-1;
									}
								}
								if(ac>0xff) ac=0;
							}
							printf("\n");
						}
					}
					if(csn>=lsn) csn=0;
					else csn++;
					if(f>0) break;	/* デジタルTVサービスの内容が取得できていたらループから抜ける */
				}
			}
			if(ps==188) break;	/* パケット末尾まで処理していたらループから抜ける */
		}
		if(f>0) break;	/* デジタルTVサービスの内容が取得できていたら終了する */
	}
	fclose(fp);
	return 0;
}
