

#include "cndays.hpp"
#include <ctime>
#include <cassert>

cnDate CCnDays::GetLunar(int yyyy, int mm, int dd)
{
    cnDate lunar = GetLunarPart(yyyy, mm, dd);

    //�ټ���ũ�������ɵ�֧����Ф�꣺
    CString sx[12] =
    {
        TEXT("��"),TEXT("ţ"),TEXT("��"),TEXT("��"),TEXT("��"),TEXT("��"),
        TEXT("��"),TEXT("��"),TEXT("��"),TEXT("��"),TEXT("��"),TEXT("��")
    };

    /*�ס��ҡ����������졢�������������ɡ��ﱻ��Ϊ��ʮ��ɡ���
    �ӡ�������î�������ȡ��硢δ���ꡢ�ϡ��硢��������ʮ����֧��*/

    CString tg[10] =
    {
        TEXT("��"),TEXT("��"),TEXT("��"),TEXT("��"),TEXT("��"),
        TEXT("��"),TEXT("��"),TEXT("��"),TEXT("��"),TEXT("��")
    };

    /*�ס��ҡ����������졢�������������ɡ��ﱻ��Ϊ��ʮ��ɡ���
   �ӡ�������î�������ȡ��硢δ���ꡢ�ϡ��硢��������ʮ����֧��*/

    CString dz[12] =
    {

        TEXT("��"),TEXT("��"),TEXT("��"),TEXT("î"),TEXT("��"),TEXT("��"),
        TEXT("��"),TEXT("δ"),TEXT("��"),TEXT("��"),TEXT("��"),TEXT("��")

    };

    lunar.cnYear = tg[(lunar.year - 1864) % 10];//���
    lunar.cnYear += dz[(lunar.year - 1864) % 12];//��֧
    lunar.cnYear += TEXT("��") + sx[(lunar.year - 1900) % 12] + TEXT("����");//��Ф

    //���ũ����
    if (lunar.leap)
    {
        Date ld1, ld2 = lunar;
        ld1.year = yyyy; ld1.month = mm; ld1.day = dd;

        //ũ��ǰһ���µ����һ��Ĺ�������
        ld2 = DaysAddOrSubtrack(ld1, ld2.day, false);

        //ũ��ǰһ���µ����һ���ũ������
        ld1 = GetLunarPart(ld2.year, ld2.month, ld2.day);

        if (lunar.month == ld1.month)
        {
            lunar.cnMonth = TEXT("��") + cnMonth[lunar.month - 1];
        }
        else
        {
            lunar.cnMonth = cnMonth[lunar.month - 1];
        }

    }
    else
    {
        lunar.cnMonth = cnMonth[lunar.month - 1];
    }


    //���ũ����
    lunar.cnDay = cnDays[lunar.day - 1];


    return lunar;
}

Date CCnDays::GetSolar(cnDate lunar)
{

    /*����������ֲ��ִ���*/
    int year = lunar.year,
    month = lunar.month,
    day = lunar.day;
    int byNow, xMonth, i;
    Date solar;
    byNow = (BaseData[year - 1901] & 0x001F) - 1;
    if (((BaseData[year - 1901] & 0x0060) >> 5) == 2)
        byNow += 31;
    for (i = 1; i < month; i++) {
        if ((BaseData[year - 1901] & (0x80000 >> (i - 1))) == 0) {
            byNow += 29;
        }
        else
            byNow += 30;
    }
    byNow += day;
    xMonth = (BaseData[year - 1901] & 0xf00000) >> 20;
    if (xMonth != 0) {
        if (month > xMonth
            || (month == xMonth && lunar.leap)) {
            if ((BaseData[year - 1901] & (0x80000 >> (month - 1))) == 0)
                byNow += 29;
            else
                byNow += 30;
        }
    }
    if (byNow > 366
        || (year % 4 != 0 && byNow == 365)) {
        year += 1;
        if (year % 4 == 0)
            byNow -= 366;
        else
            byNow -= 365;
    }
    for (i = 1; i <= 13; i++) {
        if (monthTotal[i] >= byNow) {
            month = i;
            break;
        }
    }
    solar.day = byNow - monthTotal[month - 1];
    solar.month = month;
    solar.year = year;

    return solar;
}

//GetDays24(int year, int index)��DyasOneToDate(int nCnDayOne)
//�����������ָ��24�����ڹ����е�����=======================================

Date CCnDays::GetDays24(int year, int index)
{
    /*�������羭�鹫ʽ����2099����Ч*/
    int y = year - 1900;

    int ymd = 365.242 * y + 6.2 + 15.22 * index - 1.9 * sin(0.262 * index);

    Date rd = DaysToDate(ymd);

    rd.year = year;

    return rd;
}/*//GetDays24(int year, int index)��DyasOneToDate(int nCnDayOne)
//�����������ָ��24�����ڹ����е�����
=========================================================================*/

bool CCnDays::IsLeapYear(int year)
{
    if (year % 400 == 0)
    {
        return true;
    }
    else if (year % 100 == 0)
    {
        return false;
    }
    else if (year % 4 == 0)
    {
        return true;
    }
    return false;
}

Date CCnDays::DaysToDate(int ndays)
{
    int m = 0; int y = 1900;
    Date d;

    while (ndays > GetDaysOfMonth(y, m))
    {
        ndays = ndays - GetDaysOfMonth(y, m);
        m = (m + 1) % 12;

        if (m == 0)y += 1;

        d.year = y;
        d.month = m + 1;
        d.day = ndays;
    }

    return d;
}

cnDate CCnDays::GetLunarPart(int yyyy, int mm, int dd)
{
    /*����������ֲ��ִ���*/
    int year = yyyy, month = mm, day = dd;
    int bySpring, bySolar, daysPerMonth;
    int index, flag;
    cnDate plunar;

    //bySpring ��¼�����뵱��Ԫ����������
    //bySolar ��¼�������뵱��Ԫ����������
    if (((BaseData[year - 1901] & 0x0060) >> 5) == 1)
        bySpring = (BaseData[year - 1901] & 0x001F) - 1;
    else
        bySpring = (BaseData[year - 1901] & 0x001F) - 1 + 31;
    bySolar = monthTotal[month - 1] + day - 1;
    if ((!(year % 4)) && (month > 2))
        bySolar++;

    //daysPerMonth��¼��С�µ����� 29 ��30
    //index ��¼���ĸ��¿�ʼ�����㡣
    //flag �����������µ����⴦��

    //�ж��������ڴ���ǰ���Ǵ��ں�
    if (bySolar >= bySpring) {//�������ڴ��ں󣨺��������죩
        bySolar -= bySpring;
        month = 1;
        index = 1;
        flag = 0;
        if ((BaseData[year - 1901] & (0x80000 >> (index - 1))) == 0)
            daysPerMonth = 29;
        else
            daysPerMonth = 30;
        while (bySolar >= daysPerMonth) {
            bySolar -= daysPerMonth;
            index++;
            if (month == ((BaseData[year - 1901] & 0xF00000) >> 20)) {
                flag = ~flag;
                if (flag == 0)
                    month++;
            }
            else
                month++;
            if ((BaseData[year - 1901] & (0x80000 >> (index - 1))) == 0)
                daysPerMonth = 29;
            else
                daysPerMonth = 30;
        }
        day = bySolar + 1;
    }
    else {//�������ڴ���ǰ
        bySpring -= bySolar;
        year--;
        month = 12;
        if (((BaseData[year - 1901] & 0xF00000) >> 20) == 0)
            index = 12;
        else
            index = 13;
        flag = 0;
        if ((BaseData[year - 1901] & (0x80000 >> (index - 1))) == 0)
            daysPerMonth = 29;
        else
            daysPerMonth = 30;
        while (bySpring > daysPerMonth) {
            bySpring -= daysPerMonth;
            index--;
            if (flag == 0)
                month--;
            if (month == ((BaseData[year - 1901] & 0xF00000) >> 20))
                flag = ~flag;
            if ((BaseData[year - 1901] & (0x80000 >> (index - 1))) == 0)
                daysPerMonth = 29;
            else
                daysPerMonth = 30;
        }

        day = daysPerMonth - bySpring + 1;
    }
    plunar.day = day;
    plunar.month = month;
    plunar.year = year;
    if (month == ((BaseData[year - 1901] & 0xF00000) >> 20))
        plunar.leap = true;
    else
        plunar.leap = false;

    return plunar;
}

Date CCnDays::DaysAddOrSubtrack(Date date1, int days, bool bAdd)
{
    if (bAdd)//��Ϊ�ӣ�
    {
        /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
        days += date1.day;

        if (IsLeapYear(date1.year))//����
        {
            /*rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr*/
            if (date1.month == 2)
            {
                if (days > 29)
                {
                    days = days % 29;
                    date1.month += (days / 29);
                    if (date1.month > 12)date1.year += (date1.month / 12);
                }
            }
            else if (date1.month == 1 || date1.month == 3 || date1.month == 5 || date1.month == 7
                || date1.month == 8 || date1.month == 10 || date1.month == 12)//����
            {
                if (days > 31)
                {
                    days = days % 31;
                    date1.month += (days % 31);
                    if (date1.month > 12)date1.year += (date1.month / 12);
                }

            }
            else if (date1.month == 4 || date1.month == 6 || date1.month == 9 || date1.month == 11)//С��
            {
                if (days > 30)
                {
                    days = days % 30;
                    date1.month += (days % 30);
                    if (date1.month > 12)date1.year += (date1.month / 12);
                }

            }
            /*rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr*/


        }
        else//ƽ��
        {
            /*pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp*/

            if (date1.month == 2)
            {
                if (days > 28)
                {
                    days = days % 28;
                    date1.month += (days % 28);
                    if (date1.month > 12)date1.year += (date1.month / 12);
                }

            }
            else if (date1.month == 1 || date1.month == 3 || date1.month == 5 || date1.month == 7
                || date1.month == 8 || date1.month == 10 || date1.month == 12)//����
            {
                if (days > 31)
                {
                    days = days % 31;
                    date1.month += (days % 31);
                    if (date1.month > 12)date1.year += (date1.month / 12);
                }

            }
            else if (date1.month == 4 || date1.month == 6 || date1.month == 9 || date1.month == 11)//С��
            {
                if (days > 30)
                {
                    days = days % 30;
                    date1.month += (days % 30);
                    if (date1.month > 12)date1.year += (date1.month / 12);
                }
            }

            /*pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp*/


        }



        /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    }
    else//��Ϊ����
    {
        /*��ѽ���ڵļ�����ô��ô�Ѹ�ֻ�ܶ�����İ취����*/

        for (int caldays = days; caldays >= 1; --caldays)
        {
            date1.day--; //�Լ�һ�졣

            if (date1.day == 0)
            {
                date1.month--;
                //������·ݣ���ƽ����������

                if (IsLeapYear(date1.year))//����
                {
                    if (date1.month == 2)
                    {
                        date1.day = 29;
                    }
                    else if (date1.month == 1 || date1.month == 3 || date1.month == 5 || date1.month == 7 || date1.month == 8
                        || date1.month == 10 || date1.month == 12)//����
                    {
                        date1.day = 31;
                    }
                    else if (date1.month == 4 || date1.month == 6 || date1.month == 9 || date1.month == 11)//С��
                    {
                        date1.day = 30;
                    }
                    else if (date1.month == 0)
                    {
                        date1.month = 12;
                        date1.day = 31;
                        date1.year--;
                    }

                }
                else//ƽ��
                {
                    if (date1.month == 2)
                    {
                        date1.day = 28;
                    }
                    else if (date1.month == 1 || date1.month == 3 || date1.month == 5 || date1.month == 7
                        || date1.month == 8 || date1.month == 10 || date1.month == 12)//����
                    {
                        date1.day = 31;
                    }
                    else if (date1.month == 4 || date1.month == 6 || date1.month == 9 || date1.month == 11)//С��
                    {
                        date1.day = 30;
                    }
                    else if (date1.month == 0)
                    {
                        date1.month = 12;
                        date1.day = 31;
                        date1.year--;
                    }

                }
            }


        } //�����ڼ���0���ƾͶ��˰ɣ�������������������������������         

         /*----------------------------------------------------------------------------------------------*/

        return date1;//������ֱ�ӷ��ؼ��ɡ�
    }

    date1.day = days;

    return date1;
}

int CCnDays::GetDaysOfMonth(int year, int month)
{
    //����year����month�µ�������

    if (IsLeapYear(year) && month == 1)
    {
        return DaysOfMonth[month] + 1;
    }

    return DaysOfMonth[month];
}
